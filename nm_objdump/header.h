#ifndef HEADER_H
#define HEADER_H

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void process_file(const char *filename, int nfiles);

#endif
