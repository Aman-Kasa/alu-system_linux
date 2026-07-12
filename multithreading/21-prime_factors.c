#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

/**
 * int_sqrt - computes the integer square root of a number without overflow
 * @n: number to compute the square root of
 *
 * Return: largest integer whose square is <= n
 */
static unsigned long int_sqrt(unsigned long n)
{
	unsigned long low = 0, high = n, mid;

	if (n < 2)
		return (n);

	while (low < high)
	{
		mid = low + (high - low) / 2 + 1;
		if (mid <= n / mid)
			low = mid;
		else
			high = mid - 1;
	}
	return (low);
}

/**
 * add_factor - allocates and appends a factor to a list
 * @list: list to append to
 * @n: factor value to add
 *
 * Return: 1 on success, 0 on failure
 */
static int add_factor(list_t *list, unsigned long n)
{
	unsigned long *factor;

	factor = malloc(sizeof(*factor));
	if (!factor)
		return (0);

	*factor = n;
	if (list_add(list, factor) == NULL)
	{
		free(factor);
		return (0);
	}
	return (1);
}

/**
 * prime_factors - computes the prime factors of a number given as a string
 * @s: string representation of the number to factorize
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

	while (n % 2 == 0)
	{
		if (!add_factor(list, 2))
		{
			list_destroy(list, free);
			free(list);
			return (NULL);
		}
		n /= 2;
	}

	lim = int_sqrt(n);
	for (i = 3; i <= lim; i += 2)
	{
		while (n % i == 0)
		{
			if (!add_factor(list, i))
			{
				list_destroy(list, free);
				free(list);
				return (NULL);
			}
			n /= i;
			lim = int_sqrt(n);
		}
	}

	if (n > 1 && !add_factor(list, n))
	{
		list_destroy(list, free);
		free(list);
		return (NULL);
	}

	return (list);
}
