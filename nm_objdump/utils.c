#include "header.h"

/**
 * swap16 - byte-swaps a 16-bit value
 * @v: value
 * @swap: 1 to swap
 * Return: swapped value
 */
uint16_t swap16(uint16_t v, int swap)
{
	return (swap ? ((v >> 8) | (v << 8)) : v);
}

/**
 * swap32 - byte-swaps a 32-bit value
 * @v: value
 * @swap: 1 to swap
 * Return: swapped value
 */
uint32_t swap32(uint32_t v, int swap)
{
	if (!swap)
		return (v);
	return (((v & 0xFF000000) >> 24) | ((v & 0x00FF0000) >> 8) |
		((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24));
}

/**
 * swap64 - byte-swaps a 64-bit value
 * @v: value
 * @swap: 1 to swap
 * Return: swapped value
 */
uint64_t swap64(uint64_t v, int swap)
{
	if (!swap)
		return (v);
	return (((uint64_t)swap32(v & 0xFFFFFFFF, 1) << 32) |
		swap32(v >> 32, 1));
}

/**
 * get_type - returns nm-style type char for a symbol
 * @bind: STB_* binding value
 * @shndx: section index
 * @sh_flags: section flags
 * @sh_type: section type
 * Return: symbol type character
 */
char get_type(unsigned char bind, uint16_t shndx,
	      uint64_t sh_flags, uint32_t sh_type)
{
	char c;

	if (shndx == SHN_UNDEF)
		c = 'U';
	else if (shndx == SHN_ABS)
		c = 'A';
	else if (shndx == SHN_COMMON)
		c = 'C';
	else if (sh_type == SHT_INIT_ARRAY || sh_type == SHT_FINI_ARRAY)
		c = 'T';
	else if (sh_flags & SHF_EXECINSTR)
		c = 'T';
	else if (sh_type == SHT_NOBITS)
		c = 'B';
	else if (sh_flags & SHF_WRITE)
		c = 'D';
	else
		c = 'R';

	if (bind == STB_WEAK)
		return ((c == 'U') ? 'w' : 'W');
	if (bind == STB_LOCAL && c != 'U')
		return (c | 0x20);
	return (c);
}
