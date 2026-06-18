#include "header.h"

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
					printf("%016lx %c %s\n", syms[j].st_value,
					       get_type64(&syms[j], shdr),
					       strtab + syms[j].st_name);
			}
		}
	}
}

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
					printf("%08x T %s\n", syms[j].st_value, strtab + syms[j].st_name);
			}
		}
	}
}
