#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * is_elf - checks if file is elf
 * @ptr: memory pointer
 * Return: 1 if elf, 0 otherwise
 */
int is_elf(void *ptr)
{
	Elf64_Ehdr *h = (Elf64_Ehdr *)ptr;

	return (h->e_ident[EI_MAG0] == ELFMAG0 &&
		h->e_ident[EI_MAG1] == ELFMAG1 &&
		h->e_ident[EI_MAG2] == ELFMAG2 &&
		h->e_ident[EI_MAG3] == ELFMAG3);
}

/**
 * get_type64 - returns type char for 64-bit sym
 * @sym: symbol pointer
 * @shdr: section header pointer
 * Return: char type
 */
char get_type64(Elf64_Sym *sym, Elf64_Shdr *shdr)
{
	if (sym->st_shndx == SHN_UNDEF)
		return ('U');
	if (sym->st_shndx == SHN_ABS)
		return ('A');
	if (sym->st_shndx == SHN_COMMON)
		return ('C');
	if (shdr[sym->st_shndx].sh_flags & SHF_EXECINSTR)
		return ('T');
	if (shdr[sym->st_shndx].sh_flags & SHF_WRITE)
		return ('D');
	return ('R');
}

/**
 * process_elf64 - parses 64-bit ELF sections
 * @ptr: pointer to mapped memory
 */
void process_elf64(void *ptr)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)ptr;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)ptr + ehdr->e_shoff);
	int i, j;

	for (i = 0; i < ehdr->e_shnum; i++)
	{
		if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM)
		{
			Elf64_Sym *syms = (Elf64_Sym *)((char *)ptr + shdr[i].sh_offset);
			char *strtab = (char *)ptr + shdr[shdr[i].sh_link].sh_offset;
			int num = shdr[i].sh_size / sizeof(Elf64_Sym);

			for (j = 0; j < num; j++)
			{
				if (syms[j].st_name == 0)
					continue;
				if (syms[j].st_shndx == SHN_UNDEF)
					printf("                 U %s\n",
					       strtab + syms[j].st_name);
				else
					printf("%016lx %c %s\n", syms[j].st_value,
					       get_type64(&syms[j], shdr),
					       strtab + syms[j].st_name);
			}
		}
	}
}

/**
 * process_elf32 - parses 32-bit ELF sections
 * @ptr: pointer to mapped memory
 */
void process_elf32(void *ptr)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)ptr;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((char *)ptr + ehdr->e_shoff);
	int i, j;

	for (i = 0; i < ehdr->e_shnum; i++)
	{
		if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM)
		{
			Elf32_Sym *syms = (Elf32_Sym *)((char *)ptr + shdr[i].sh_offset);
			char *strtab = (char *)ptr + shdr[shdr[i].sh_link].sh_offset;
			int num = shdr[i].sh_size / sizeof(Elf32_Sym);

			for (j = 0; j < num; j++)
			{
				if (syms[j].st_name == 0)
					continue;
				if (syms[j].st_shndx == SHN_UNDEF)
					printf("         U %s\n", strtab + syms[j].st_name);
				else
					printf("%08x T %s\n", syms[j].st_value,
					       strtab + syms[j].st_name);
			}
		}
	}
}

/**
 * process_file - maps file into memory and calls processor
 * @filename: file path
 */
void process_file(const char *filename)
{
	int fd = open(filename, O_RDONLY);
	struct stat st;
	void *ptr;

	if (fd < 0 || fstat(fd, &st) < 0)
		return;
	ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr != MAP_FAILED)
	{
		if (((unsigned char *)ptr)[EI_CLASS] == ELFCLASS64)
			process_elf64(ptr);
		else
			process_elf32(ptr);
		munmap(ptr, st.st_size);
	}
	close(fd);
}

/**
 * main - entry point
 * @ac: count
 * @av: vector
 * Return: 0
 */
int main(int ac, char **av)
{
	if (ac < 2)
		return (1);
	process_file(av[1]);
	return (0);
}
