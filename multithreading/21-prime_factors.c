#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

/**
 * lim_binary - finds the integer square root of a number
 * @n: the number to compute the square root of
 *
 * Return: the integer square root of n
 */
static unsigned long lim_binary(unsigned long n)
{
	unsigned long low = 0, high = n, mid;

	while (low <= high)
	{
		mid = (low + high) / 2;
		if (mid * mid <= n)
			low = mid + 1;
		else
			high = mid - 1;
	}
	return (high);
}

/**
 * prime_factors - computes the prime factors of a number given as a string
 * @s: string representing the number to factorize
 *
 * Return: pointer to a list_t containing the prime factors, or NULL on failure
 */
list_t *prime_factors(char const *s)
{
	list_t *list;
	unsigned long n, i, lim;

	if (!s)
		return (NULL);

	list = malloc(sizeof(*list));
	if (!list)
		return (NULL);
	list_init(list);

	n = strtoul(s, NULL, 10);

	/* handle factor 2 separately to optimize */
	while (n % 2 == 0)
	{
		unsigned long *factor = malloc(sizeof(*factor));

		if (!factor || list_add(list, factor) == NULL)
		{
			list_destroy(list, free);
			free(list);
			return (NULL);
		}
		*factor = 2;
		n /= 2;
	}

	/* check odd factors up to sqrt(n) */
	lim = lim_binary(n);
	for (i = 3; i <= lim; i += 2)
	{
		while (n % i == 0)
		{
			unsigned long *factor = malloc(sizeof(*factor));

			if (!factor || list_add(list, factor) == NULL)
			{
				list_destroy(list, free);
				free(list);
				return (NULL);
			}
			*factor = i;
			n /= i;
			lim = lim_binary(n);
		}
	}

	/* if n is still greater than 1, it is a prime factor */
	if (n > 1)
	{
		unsigned long *factor = malloc(sizeof(*factor));

		if (!factor || list_add(list, factor) == NULL)
		{
			list_destroy(list, free);
			free(list);
			return (NULL);
		}
		*factor = n;
	}

	return (list);
}
