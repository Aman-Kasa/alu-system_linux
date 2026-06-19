#include "objdump.h"

/**
 * hobjdump_file - maps and processes one ELF file
 * @filename: path to file
 */
void hobjdump_file(const char *filename)
{
	int fd;
	struct stat st;
	void *ptr;
	unsigned char *e;

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
	if (e[EI_CLASS] == ELFCLASS64)
		hobjdump64(ptr, filename);
	else
		hobjdump32(ptr, filename);
	munmap(ptr, st.st_size);
}

/**
 * main - entry point
 * @ac: argument count
 * @av: argument vector
 * Return: 0
 */
int main(int ac, char **av)
{
	int i;

	if (ac < 2)
		return (1);
	for (i = 1; i < ac; i++)
		hobjdump_file(av[i]);
	return (0);
}
