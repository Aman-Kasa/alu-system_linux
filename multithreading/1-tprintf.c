#include "multithreading.h"
#include <stdio.h>

/**
 * tprintf - Print a formatted string preceded by the calling thread ID
 * @format: Format string
 *
 * Return: Number of characters printed
 */
int tprintf(char const *format, ...)
{
	va_list args;
	int ret;

	va_start(args, format);
	ret = printf("[%lu] ", pthread_self());
	ret += vprintf(format, args);
	va_end(args);

	return (ret);
}
