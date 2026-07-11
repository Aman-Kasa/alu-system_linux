#include "multithreading.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * prime_factors - Factorise a number into a list of prime factors
 * @s: String representation of the number
 *
 * Return: Pointer to a list of unsigned long factors, or NULL on failure
 */
list_t *prime_factors(char const *s)
{
	unsigned long n, factor;
	char *endptr;
	list_t *list;
	unsigned long *p;

	n = strtoul(s, &endptr, 10);
	if (*endptr != '\0')
		return (NULL);

	list = malloc(sizeof(list_t));
	if (!list)
		return (NULL);
	if (!list_init(list))
	{
		free(list);
		return (NULL);
	}

	for (factor = 2; factor * factor <= n; factor++)
	{
		while (n % factor == 0)
		{
			p = malloc(sizeof(unsigned long));
			if (!p)
			{
				list_destroy(list, free);
				free(list);
				return (NULL);
			}
			*p = factor;
			list_add(list, p);
			n /= factor;
		}
	}
	if (n > 1)
	{
		p = malloc(sizeof(unsigned long));
		if (!p)
		{
			list_destroy(list, free);
			free(list);
			return (NULL);
		}
		*p = n;
		list_add(list, p);
	}

	return (list);
}
