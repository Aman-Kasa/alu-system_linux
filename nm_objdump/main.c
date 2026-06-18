#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * is_elf - verifies if the mapped file is an ELF file
 * @ptr: pointer to the mapped memory
 * Return: 1 if true, 0 otherwise
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
					printf("                 U %s\n", strtab + syms[j].st_name);
				else
					printf("%016lx T %s\n", syms[j].st_value, strtab + syms[j].st_name);
			}
		}
	}
}

/* Add process_elf32, process_file, and main following these same indentation rules */
