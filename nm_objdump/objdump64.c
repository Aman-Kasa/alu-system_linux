#include "objdump.h"

/**
 * arch_str64 - returns architecture string for 64-bit ELF
 * @machine: e_machine value
 * Return: architecture string
 */
static const char *arch_str64(uint16_t machine)
{
	if (machine == EM_X86_64)
		return ("i386:x86-64");
	if (machine == EM_AARCH64)
		return ("aarch64");
	if (machine == EM_PPC64)
		return ("powerpc:common64");
	if (machine == EM_SPARCV9)
		return ("sparc:v9a");
	return ("unknown");
}

/**
 * fmt_str64 - returns file format string for 64-bit ELF
 * @machine: e_machine value
 * @endian: EI_DATA value
 * Return: format string
 */
static const char *fmt_str64(uint16_t machine, unsigned char endian)
{
	if (machine == EM_X86_64)
		return ("elf64-x86-64");
	if (machine == EM_AARCH64)
		return ("elf64-littleaarch64");
	if (machine == EM_PPC64)
		return (endian == ELFDATA2MSB ? "elf64-powerpc" : "elf64-powerpcle");
	if (machine == EM_SPARCV9)
		return ("elf64-sparc");
	return ("elf64-unknown");
}

/**
 * print_flags64 - prints ELF flags header block
 * @ehdr: ELF header
 * @has_syms: 1 if .symtab exists
 * @sw: swap flag
 */
static void print_flags64(Elf64_Ehdr *ehdr, int has_syms, int sw)
{
	uint16_t type = od_swap16(ehdr->e_type, sw);
	uint64_t entry = od_swap64(ehdr->e_entry, sw);
	uint32_t flags = 0x100;
	const char *sep = "";

	if (type == ET_EXEC)
		flags |= 0x02;
	if (has_syms)
		flags |= 0x10;
	if (type == ET_DYN)
		flags |= 0x40;
	printf("architecture: %s, flags 0x%08x:\n",
	       arch_str64(od_swap16(ehdr->e_machine, sw)), flags);
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
	printf("%sD_PAGED", sep);
	printf("\nstart address 0x%016lx\n\n", entry);
}

/**
 * hobjdump64 - prints objdump -sf output for a 64-bit ELF
 * @ptr: pointer to mapped ELF
 * @filename: file name
 */
void hobjdump64(void *ptr, const char *filename)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)ptr;
	int sw = (((unsigned char *)ptr)[EI_DATA] == ELFDATA2MSB);
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)ptr +
					   od_swap64(ehdr->e_shoff, sw));
	uint16_t shnum = od_swap16(ehdr->e_shnum, sw);
	uint16_t shstrndx = od_swap16(ehdr->e_shstrndx, sw);
	char *shstrtab = (char *)ptr +
		od_swap64(shdr[shstrndx].sh_offset, sw);
	int has_syms = 0, i;
	uint64_t sh_size, sh_off, sh_addr;
	uint32_t sh_type;
	const char *sec_name;

	for (i = 0; i < shnum; i++)
		if (od_swap32(shdr[i].sh_type, sw) == SHT_SYMTAB)
		{
			has_syms = 1;
			break;
		}
	printf("\n%s:     file format %s\n",
	       filename, fmt_str64(od_swap16(ehdr->e_machine, sw),
				   ((unsigned char *)ptr)[EI_DATA]));
	print_flags64(ehdr, has_syms, sw);
	for (i = 0; i < shnum; i++)
	{
		sh_type = od_swap32(shdr[i].sh_type, sw);
		sh_size = od_swap64(shdr[i].sh_size, sw);
		sh_off = od_swap64(shdr[i].sh_offset, sw);
		sh_addr = od_swap64(shdr[i].sh_addr, sw);
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
