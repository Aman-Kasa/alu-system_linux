#include "header.h"

/**
 * is_elf - checks if mapped memory is an ELF file
 * @ptr: pointer to mapped memory
 * Return: 1 if ELF, 0 otherwise
 */
int is_elf(void *ptr)
{
	unsigned char *e = (unsigned char *)ptr;

	return (e[EI_MAG0] == ELFMAG0 && e[EI_MAG1] == ELFMAG1 &&
		e[EI_MAG2] == ELFMAG2 && e[EI_MAG3] == ELFMAG3);
}

/**
 * main - entry point
 * @ac: argument count
 * @av: argument vector
 * Return: 0 on success, 1 on error
 */
int main(int ac, char **av)
{
	int i;

	if (ac < 2)
		return (1);
	for (i = 1; i < ac; i++)
		process_file(av[i], ac - 1);
	return (0);
}
