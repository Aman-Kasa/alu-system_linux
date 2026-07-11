#include "multithreading.h"
#include <stdlib.h>

/**
 * prime_factors - Factorizes a number into a list of prime factors
 * @s: String representation of the number to factorize
 *
 * Return: Pointer to a list of unsigned long factors, or NULL on failure
 */
list_t *prime_factors(char const *s)
{
	unsigned long n, factor = 2, *p;
	char *endptr;
	list_t *list;

	if (!s)
		return (NULL);

	n = strtoul(s, &endptr, 10);
	if (*endptr != '\0')
		return (NULL);

	/* calloc zeros out the struct, handling head/tail initialization */
	list = calloc(1, sizeof(list_t));
	if (!list)
		return (NULL);

	while (n >= 2)
	{
		if (n % factor == 0)
		{
			p = malloc(sizeof(unsigned long));
			if (!p)
				return (NULL);
			*p = factor;
			list_add(list, p);
			n /= factor;
		}
		else
		{
			/* Step by 1 for even, step by 2 for odd primes */
			factor += (factor == 2) ? 1 : 2;
			
			/* Skip unnecessary loops by jumping straight to the final prime */
			if (factor * factor > n && n > 1)
				factor = n;
		}
	}

	return (list);
}
