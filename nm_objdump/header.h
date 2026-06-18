#ifndef HEADER_H
#define HEADER_H

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

uint16_t swap16(uint16_t v, int swap);
uint32_t swap32(uint32_t v, int swap);
uint64_t swap64(uint64_t v, int swap);
char get_type(unsigned char bind, uint16_t shndx,
	      uint64_t sh_flags, uint32_t sh_type, const char *sec_name);
void print_sym64(Elf64_Sym *sym, char *strtab, Elf64_Shdr *shdr,
		 char *shstrtab, int swap);
void process_elf64(void *ptr, int swap, const char *filename);
void print_sym32(Elf32_Sym *sym, char *strtab, Elf32_Shdr *shdr,
		 char *shstrtab, int swap);
void process_elf32(void *ptr, int swap, const char *filename);
void process_file(const char *filename, int nfiles);

#endif
