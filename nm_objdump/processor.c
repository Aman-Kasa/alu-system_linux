#include "header.h"

/**
 * print_sym64 - prints one 64-bit symbol
 * @sym: pointer to symbol
 * @strtab: string table
 * @shdr: section headers
 * @shstrtab: section name string table
 * @swap: endian swap flag
 */
void print_sym64(Elf64_Sym *sym, char *strtab, Elf64_Shdr *shdr,
		 char *shstrtab, int swap)
{
	uint16_t shndx = swap16(sym->st_shndx, swap);
	uint64_t val = swap64(sym->st_value, swap);
	uint32_t sh_type = 0;
	uint64_t sh_flags = 0;
	const char *sec_name = NULL;
	char t;

	if (shndx < SHN_LORESERVE)
	{
		sh_type = swap32(shdr[shndx].sh_type, swap);
		sh_flags = swap64(shdr[shndx].sh_flags, swap);
		if (shstrtab)
			sec_name = shstrtab + swap32(shdr[shndx].sh_name, swap);
	}
	t = get_type(ELF64_ST_BIND(sym->st_info), shndx, sh_flags,
		     sh_type, sec_name);
	if (shndx == SHN_UNDEF)
		printf("                 %c %s\n", t,
		       strtab + swap32(sym->st_name, swap));
	else
		printf("%016lx %c %s\n", val, t,
		       strtab + swap32(sym->st_name, swap));
}

/**
 * process_elf64 - prints symbols from a 64-bit ELF
 * @ptr: pointer to mapped ELF data
 * @swap: 1 if byte-swapping needed
 * @filename: file name for error messages
 */
void process_elf64(void *ptr, int swap, const char *filename)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)ptr;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)ptr +
					   swap64(ehdr->e_shoff, swap));
	uint16_t shnum = swap16(ehdr->e_shnum, swap);
	uint16_t shstrndx = swap16(ehdr->e_shstrndx, swap);
	char *shstrtab = (char *)ptr +
		swap64(shdr[shstrndx].sh_offset, swap);
	Elf64_Sym *syms;
	char *strtab;
	int i, j, num, si = -1;

	for (i = 0; i < shnum; i++)
		if (swap32(shdr[i].sh_type, swap) == SHT_SYMTAB)
		{
			si = i;
			break;
		}
	if (si == -1)
	{
		dprintf(2, "./hnm: %s: no symbols\n", filename);
		return;
	}
	syms = (Elf64_Sym *)((char *)ptr + swap64(shdr[si].sh_offset, swap));
	strtab = (char *)ptr +
		swap64(shdr[swap32(shdr[si].sh_link, swap)].sh_offset, swap);
	num = swap64(shdr[si].sh_size, swap) / sizeof(Elf64_Sym);
	for (j = 0; j < num; j++)
	{
		if (syms[j].st_name == 0 ||
		    ELF64_ST_TYPE(syms[j].st_info) == STT_FILE)
			continue;
		print_sym64(&syms[j], strtab, shdr, shstrtab, swap);
	}
}

/**
 * print_sym32 - prints one 32-bit symbol
 * @sym: pointer to symbol
 * @strtab: string table
 * @shdr: section headers
 * @shstrtab: section name string table
 * @swap: endian swap flag
 */
void print_sym32(Elf32_Sym *sym, char *strtab, Elf32_Shdr *shdr,
		 char *shstrtab, int swap)
{
	uint16_t shndx = swap16(sym->st_shndx, swap);
	uint32_t val = swap32(sym->st_value, swap);
	uint32_t sh_type = 0, sh_flags = 0;
	const char *sec_name = NULL;
	char t;

	if (shndx < SHN_LORESERVE)
	{
		sh_type = swap32(shdr[shndx].sh_type, swap);
		sh_flags = swap32(shdr[shndx].sh_flags, swap);
		if (shstrtab)
			sec_name = shstrtab + swap32(shdr[shndx].sh_name, swap);
	}
	t = get_type(ELF32_ST_BIND(sym->st_info), shndx, sh_flags,
		     sh_type, sec_name);
	if (shndx == SHN_UNDEF)
		printf("         %c %s\n", t,
		       strtab + swap32(sym->st_name, swap));
	else
		printf("%08x %c %s\n", val, t,
		       strtab + swap32(sym->st_name, swap));
}

/**
 * process_elf32 - prints symbols from a 32-bit ELF
 * @ptr: pointer to mapped ELF data
 * @swap: 1 if byte-swapping needed
 * @filename: file name for error messages
 */
void process_elf32(void *ptr, int swap, const char *filename)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ptr;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((char *)ptr +
					   swap32(ehdr->e_shoff, swap));
	uint16_t shnum = swap16(ehdr->e_shnum, swap);
	uint16_t shstrndx = swap16(ehdr->e_shstrndx, swap);
	char *shstrtab = (char *)ptr +
		swap32(shdr[shstrndx].sh_offset, swap);
	Elf32_Sym *syms;
	char *strtab;
	int i, j, num, si = -1;

	for (i = 0; i < shnum; i++)
		if (swap32(shdr[i].sh_type, swap) == SHT_SYMTAB)
		{
			si = i;
			break;
		}
	if (si == -1)
	{
		dprintf(2, "./hnm: %s: no symbols\n", filename);
		return;
	}
	syms = (Elf32_Sym *)((char *)ptr + swap32(shdr[si].sh_offset, swap));
	strtab = (char *)ptr +
		swap32(shdr[swap32(shdr[si].sh_link, swap)].sh_offset, swap);
	num = swap32(shdr[si].sh_size, swap) / sizeof(Elf32_Sym);
	for (j = 0; j < num; j++)
	{
		if (syms[j].st_name == 0 ||
		    ELF32_ST_TYPE(syms[j].st_info) == STT_FILE)
			continue;
		print_sym32(&syms[j], strtab, shdr, shstrtab, swap);
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
	unsigned char *e;
	int swap;

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
	e = (unsigned char *)ptr;
	swap = (e[EI_DATA] == ELFDATA2MSB);
	if (nfiles > 1)
		printf("\n%s:\n", filename);
	if (e[EI_CLASS] == ELFCLASS64)
		process_elf64(ptr, swap, filename);
	else
		process_elf32(ptr, swap, filename);
	munmap(ptr, st.st_size);
}
