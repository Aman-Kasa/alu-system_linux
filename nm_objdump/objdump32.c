#include "objdump.h"

/**
 * arch_str32 - returns architecture string for 32-bit ELF
 * @machine: e_machine value
 * @endian: EI_DATA value
 * Return: architecture string
 */
static const char *arch_str32(uint16_t machine, unsigned char endian)
{
	if (endian == ELFDATA2MSB)
		return ("UNKNOWN!");
	if (machine == EM_386)
		return ("i386");
	if (machine == EM_ARM)
		return ("arm");
	if (machine == EM_PPC)
		return ("powerpc:common");
	if (machine == EM_MIPS)
		return ("mips:3000");
	return ("UNKNOWN!");
}

/**
 * fmt_str32 - returns file format string for 32-bit ELF
 * @machine: e_machine value
 * @endian: EI_DATA value
 * Return: format string
 */
static const char *fmt_str32(uint16_t machine, unsigned char endian)
{
	if (endian == ELFDATA2MSB)
		return ("elf32-big");
	if (machine == EM_386)
		return ("elf32-i386");
	if (machine == EM_ARM)
		return ("elf32-littlearm");
	if (machine == EM_PPC)
		return ("elf32-powerpc");
	if (machine == EM_MIPS)
		return ("elf32-littlemips");
	return ("elf32-little");
}

/**
 * print_flags32 - prints ELF flags header block for 32-bit
 * @ehdr: ELF header
 * @has_syms: 1 if .symtab exists
 * @sw: swap flag
 */
static void print_flags32(Elf32_Ehdr *ehdr, int has_syms, int sw,
			  unsigned char endian)
{
	uint16_t type = od_swap16(ehdr->e_type, sw);
	uint32_t entry = od_swap32(ehdr->e_entry, sw);
	uint32_t flags = 0;
	const char *sep = "";

	if (type == ET_REL)
		flags |= 0x01;
	if (type == ET_EXEC)
		flags |= 0x02;
	if (has_syms)
		flags |= 0x10;
	if (type == ET_DYN)
		flags |= 0x40;
	if (type != ET_REL)
		flags |= 0x100;
	printf("architecture: %s, flags 0x%08x:\n",
	       arch_str32(od_swap16(ehdr->e_machine, sw), endian), flags);
	if (type == ET_REL)
	{
		printf("%sHAS_RELOC", sep);
		sep = ", ";
	}
	if (type == ET_EXEC)
	{
		printf("%sEXEC_P", sep);
		sep = ", ";
	}
	if (has_syms)
	{
		printf("%sHAS_SYMS", sep);
		sep = ", ";
	}
	if (type == ET_DYN)
	{
		printf("%sDYNAMIC", sep);
		sep = ", ";
	}
	if (type != ET_REL)
		printf("%sD_PAGED", sep);
	printf("\nstart address 0x%08x\n\n", entry);
}

/**
 * hobjdump32 - prints objdump -sf output for a 32-bit ELF
 * @ptr: pointer to mapped ELF
 * @filename: file name
 */
void hobjdump32(void *ptr, const char *filename)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ptr;
	int sw = (((unsigned char *)ptr)[EI_DATA] == ELFDATA2MSB);
	Elf32_Shdr *shdr = (Elf32_Shdr *)((char *)ptr +
					   od_swap32(ehdr->e_shoff, sw));
	uint16_t shnum = od_swap16(ehdr->e_shnum, sw);
	uint16_t shstrndx = od_swap16(ehdr->e_shstrndx, sw);
	char *shstrtab = (char *)ptr +
		od_swap32(shdr[shstrndx].sh_offset, sw);
	int has_syms = 0, i;
	uint32_t sh_size, sh_off, sh_addr, sh_type;
	const char *sec_name;

	for (i = 0; i < shnum; i++)
	{
		if (od_swap32(shdr[i].sh_type, sw) == SHT_SYMTAB ||
		    od_swap32(shdr[i].sh_type, sw) == SHT_DYNSYM)
		{
			has_syms = 1;
			break;
		}
	}
	printf("\n%s:     file format %s\n",
	       filename, fmt_str32(od_swap16(ehdr->e_machine, sw),
				   ((unsigned char *)ptr)[EI_DATA]));
	print_flags32(ehdr, has_syms, sw, ((unsigned char *)ptr)[EI_DATA]);
	for (i = 0; i < shnum; i++)
	{
		sh_type = od_swap32(shdr[i].sh_type, sw);
		sh_size = od_swap32(shdr[i].sh_size, sw);
		sh_off = od_swap32(shdr[i].sh_offset, sw);
		sh_addr = od_swap32(shdr[i].sh_addr, sw);
		sec_name = shstrtab + od_swap32(shdr[i].sh_name, sw);
		if (sh_type == SHT_NULL || sh_type == SHT_NOBITS || sh_size == 0)
			continue;
		if (sh_type == SHT_SYMTAB)
			continue;
		if (!strcmp(sec_name, ".strtab") || !strcmp(sec_name, ".shstrtab"))
			continue;
		printf("Contents of section %s:\n", sec_name);
		od_hexdump(sh_addr, (unsigned char *)ptr + sh_off, sh_size);
	}
}
