#ifndef HEADER_H
#define HEADER_H

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void process_elf64(void *ptr);
void process_elf32(void *ptr);
void process_file(const char *filename);
char get_type64(Elf64_Sym *sym, Elf64_Shdr *shdr);

#endif
