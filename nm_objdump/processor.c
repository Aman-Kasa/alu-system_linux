#include "header.h"

/**
 * get_type - returns nm-style type char for a symbol
 * @bind: STB_* binding value
 * @shndx: section index
 * @sh_flags: section flags
 * @sh_type: section type
 * Return: symbol type character
 */
static char get_type(unsigned char bind, Elf64_Half shndx,
		     Elf64_Xword sh_flags, Elf64_Word sh_type)
{
	char c;

	if (shndx == SHN_UNDEF)
		c = 'U';
	else if (shndx == SHN_ABS)
		c = 'A';
	else if (shndx == SHN_COMMON)
		c = 'C';
	else if (sh_flags & SHF_EXECINSTR)
		c = 'T';
	else if (sh_type == SHT_NOBITS)
		c = 'B';
	else if (sh_flags & SHF_WRITE)
		c = 'D';
	else
		c = 'R';

	if (bind == STB_WEAK)
		return ((c == 'U') ? 'w' : 'W');
	if (bind == STB_LOCAL && c != 'U')
		return (c | 0x20);
	return (c);
}

/**
 * process_elf64 - prints symbols from a 64-bit ELF
 * @ptr: pointer to mapped ELF data
 */
void process_elf64(void *ptr)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)ptr;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)ptr + ehdr->e_shoff);
	Elf64_Sym *syms;
	char *strtab;
	char t;
	int i, j, num, si = -1;

	for (i = 0; i < ehdr->e_shnum; i++)
		if (shdr[i].sh_type == SHT_SYMTAB)
		{
			si = i;
			break;
		}
	if (si == -1)
		for (i = 0; i < ehdr->e_shnum; i++)
			if (shdr[i].sh_type == SHT_DYNSYM)
			{
				si = i;
				break;
			}
	if (si == -1)
		return;
	syms = (Elf64_Sym *)((char *)ptr + shdr[si].sh_offset);
	strtab = (char *)ptr + shdr[shdr[si].sh_link].sh_offset;
	num = shdr[si].sh_size / sizeof(Elf64_Sym);
	for (j = 0; j < num; j++)
	{
		if (syms[j].st_name == 0)
			continue;
		if (ELF64_ST_TYPE(syms[j].st_info) == STT_FILE)
			continue;
		t = get_type(ELF64_ST_BIND(syms[j].st_info),
			     syms[j].st_shndx,
			     (syms[j].st_shndx < SHN_LORESERVE)
			     ? shdr[syms[j].st_shndx].sh_flags : 0,
			     (syms[j].st_shndx < SHN_LORESERVE)
			     ? shdr[syms[j].st_shndx].sh_type : 0);
		if (syms[j].st_shndx == SHN_UNDEF)
			printf("                 %c %s\n", t,
			       strtab + syms[j].st_name);
		else
			printf("%016lx %c %s\n", syms[j].st_value, t,
			       strtab + syms[j].st_name);
	}
}

/**
 * process_elf32 - prints symbols from a 32-bit ELF
 * @ptr: pointer to mapped ELF data
 */
void process_elf32(void *ptr)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ptr;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((char *)ptr + ehdr->e_shoff);
	Elf32_Sym *syms;
	char *strtab;
	char t;
	int i, j, num, si = -1;

	for (i = 0; i < ehdr->e_shnum; i++)
		if (shdr[i].sh_type == SHT_SYMTAB)
		{
			si = i;
			break;
		}
	if (si == -1)
		for (i = 0; i < ehdr->e_shnum; i++)
			if (shdr[i].sh_type == SHT_DYNSYM)
			{
				si = i;
				break;
			}
	if (si == -1)
		return;
	syms = (Elf32_Sym *)((char *)ptr + shdr[si].sh_offset);
	strtab = (char *)ptr + shdr[shdr[si].sh_link].sh_offset;
	num = shdr[si].sh_size / sizeof(Elf32_Sym);
	for (j = 0; j < num; j++)
	{
		if (syms[j].st_name == 0)
			continue;
		if (ELF32_ST_TYPE(syms[j].st_info) == STT_FILE)
			continue;
		t = get_type(ELF32_ST_BIND(syms[j].st_info),
			     syms[j].st_shndx,
			     (syms[j].st_shndx < SHN_LORESERVE)
			     ? shdr[syms[j].st_shndx].sh_flags : 0,
			     (syms[j].st_shndx < SHN_LORESERVE)
			     ? shdr[syms[j].st_shndx].sh_type : 0);
		if (syms[j].st_shndx == SHN_UNDEF)
			printf("         %c %s\n", t,
			       strtab + syms[j].st_name);
		else
			printf("%08x %c %s\n", syms[j].st_value, t,
			       strtab + syms[j].st_name);
	}
}

/**
 * process_file - maps an ELF file and dispatches to the right processor
 * @filename: path to the ELF file
 * @nfiles: total number of files being processed
 */
void process_file(const char *filename, int nfiles)
{
	int fd;
	struct stat st;
	void *ptr;

	fd = open(filename, O_RDONLY);
	if (fd < 0 || fstat(fd, &st) < 0)
	{
		if (fd >= 0)
			close(fd);
		return;
	}
	ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (ptr == MAP_FAILED)
		return;
	if (nfiles > 1)
		printf("\n%s:\n", filename);
	if (((unsigned char *)ptr)[EI_CLASS] == ELFCLASS64)
		process_elf64(ptr);
	else
		process_elf32(ptr);
	munmap(ptr, st.st_size);
}
