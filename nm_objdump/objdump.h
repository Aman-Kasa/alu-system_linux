#ifndef OBJDUMP_H
#define OBJDUMP_H

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

uint16_t od_swap16(uint16_t v, int sw);
uint32_t od_swap32(uint32_t v, int sw);
uint64_t od_swap64(uint64_t v, int sw);
void od_hexdump(uint64_t addr, const unsigned char *data, uint64_t size);
void hobjdump64(void *ptr, const char *filename);
void hobjdump32(void *ptr, const char *filename);
void hobjdump_file(const char *filename);

#endif
