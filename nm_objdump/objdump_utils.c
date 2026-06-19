#include "objdump.h"

/**
 * od_swap16 - byte-swaps a 16-bit value
 * @v: value
 * @sw: 1 to swap
 * Return: swapped value
 */
uint16_t od_swap16(uint16_t v, int sw)
{
	return (sw ? ((v >> 8) | (v << 8)) : v);
}

/**
 * od_swap32 - byte-swaps a 32-bit value
 * @v: value
 * @sw: 1 to swap
 * Return: swapped value
 */
uint32_t od_swap32(uint32_t v, int sw)
{
	if (!sw)
		return (v);
	return (((v & 0xFF000000) >> 24) | ((v & 0x00FF0000) >> 8) |
		((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24));
}

/**
 * od_swap64 - byte-swaps a 64-bit value
 * @v: value
 * @sw: 1 to swap
 * Return: swapped value
 */
uint64_t od_swap64(uint64_t v, int sw)
{
	if (!sw)
		return (v);
	return (((uint64_t)od_swap32(v & 0xFFFFFFFF, 1) << 32) |
		od_swap32(v >> 32, 1));
}

/**
 * od_hexdump - prints hex dump in objdump -s format
 * @addr: starting address of this data
 * @data: pointer to raw bytes
 * @size: number of bytes
 */
void od_hexdump(uint64_t addr, const unsigned char *data, uint64_t size)
{
	uint64_t i, j;
	unsigned char byte;

	for (i = 0; i < size; i += 16)
	{
		printf(" %04lx", addr + i);
		for (j = 0; j < 16; j++)
		{
			if (j % 4 == 0)
				printf(" ");
			if (i + j < size)
				printf("%02x", data[i + j]);
			else
				printf("  ");
		}
		printf("  ");
		for (j = 0; j < 16; j++)
		{
			if (i + j < size)
			{
				byte = data[i + j];
				printf("%c", (isprint(byte) ? byte : '.'));
			}
			else
				printf(" ");
		}
		printf("\n");
	}
}
