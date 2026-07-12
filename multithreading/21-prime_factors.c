#include "multithreading.h"
#include <stdlib.h>
#include <string.h>

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
 * addmod - computes (a + b) % m without overflowing unsigned long
 * @a: first operand (must be < m)
 * @b: second operand (must be < m)
 * @m: modulus
 *
 * Return: (a + b) mod m
 */
static unsigned long addmod(unsigned long a, unsigned long b, unsigned long m)
{
	if (b >= m - a)
		return (a - (m - b));
	return (a + b);
}

/**
 * mulmod - computes (a * b) % m without overflowing unsigned long
 * @a: first operand
 * @b: second operand
 * @m: modulus
 *
 * Return: (a * b) mod m
 */
static unsigned long mulmod(unsigned long a, unsigned long b, unsigned long m)
{
	unsigned long result = 0;

	a %= m;
	while (b > 0)
	{
		if (b & 1)
			result = addmod(result, a, m);
		a = addmod(a, a, m);
		b >>= 1;
	}
	return (result);
}

/**
 * powmod - computes (base ^ exp) % mod using fast exponentiation
 * @base: base value
 * @exp: exponent
 * @mod: modulus
 *
 * Return: (base ^ exp) mod mod
 */
static unsigned long powmod(unsigned long base, unsigned long exp,
	unsigned long mod)
{
	unsigned long result = 1;

	base %= mod;
	while (exp > 0)
	{
		if (exp & 1)
			result = mulmod(result, base, mod);
		base = mulmod(base, base, mod);
		exp >>= 1;
	}
	return (result);
}
c/**
 * miller_test - runs one round of the Miller-Rabin primality test
 * @n: number being tested
 * @a: witness value
 * @d: odd part of (n - 1)
 * @r: number of times 2 divides (n - 1)
 *
 * Return: 1 if n passes this round (probably prime), 0 if composite
 */
static int miller_test(unsigned long n, unsigned long a, unsigned long d,
	int r)
{
	unsigned long x = powmod(a, d, n);
	int i;

	if (x == 1 || x == n - 1)
		return (1);
	for (i = 0; i < r - 1; i++)
	{
		x = mulmod(x, x, n);
		if (x == n - 1)
			return (1);
	}
	return (0);
}

/**
 * is_prime - deterministic primality test valid for all 64-bit values
 * @n: number to test
 *
 * Return: 1 if n is prime, 0 otherwise
 */
static int is_prime(unsigned long n)
{
	static const unsigned long w[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
	unsigned long d = n - 1;
	int r = 0, i, count = (int)(sizeof(w) / sizeof(w[0]));

	if (n < 2)
		return (0);
	for (i = 0; i < count; i++)
	{
		if (n == w[i])
			return (1);
		if (n % w[i] == 0)
			return (0);
	}
	while (d % 2 == 0)
	{
		d /= 2;
		r++;
	}
	for (i = 0; i < count; i++)
		if (!miller_test(n, w[i], d, r))
			return (0);
	return (1);
}
c/**
 * pollard_g - the pseudo-random function used by Pollard's rho
 * @x: current value
 * @c: constant offset for this attempt
 * @n: modulus (number being factored)
 *
 * Return: next value in the sequence
 */
static unsigned long pollard_g(unsigned long x, unsigned long c,
	unsigned long n)
{
	return (addmod(mulmod(x, x, n), c, n));
}

/**
 * gcd - computes the greatest common divisor of two numbers
 * @a: first number
 * @b: second number
 *
 * Return: gcd(a, b)
 */
static unsigned long gcd(unsigned long a, unsigned long b)
{
	unsigned long tmp;

	while (b)
	{
		tmp = b;
		b = a % b;
		a = tmp;
	}
	return (a);
}

/**
 * pollard_rho - finds a nontrivial factor of a composite number
 * @n: composite number to split
 *
 * Return: a factor of n (not necessarily prime)
 */
static unsigned long pollard_rho(unsigned long n)
{
	unsigned long x, y, c, d, diff;

	if (n % 2 == 0)
		return (2);

	for (c = 1; ; c++)
	{
		x = 2;
		y = 2;
		d = 1;
		while (d == 1)
		{
			x = pollard_g(x, c, n);
			y = pollard_g(pollard_g(y, c, n), c, n);
			diff = x > y ? x - y : y - x;
			d = gcd(diff, n);
		}
		if (d != n)
			return (d);
	}
}
c/**
 * collect_factors - recursively finds all prime factors of n
 * @n: number to factorize
 * @arr: pointer to the dynamic array of factors found so far
 * @cnt: pointer to the current count of factors
 * @cap: pointer to the current capacity of the array
 *
 * Return: 1 on success, 0 on allocation failure
 */
static int collect_factors(unsigned long n, unsigned long **arr, size_t *cnt,
	size_t *cap)
{
	unsigned long d;

	if (n == 1)
		return (1);

	if (is_prime(n))
	{
		if (*cnt == *cap)
		{
			*cap *= 2;
			*arr = realloc(*arr, *cap * sizeof(**arr));
			if (!*arr)
				return (0);
		}
		(*arr)[(*cnt)++] = n;
		return (1);
	}

	d = pollard_rho(n);
	return (collect_factors(d, arr, cnt, cap) &&
		collect_factors(n / d, arr, cnt, cap));
}

/**
 * cmp_ulong - comparator for sorting unsigned longs ascending
 * @a: pointer to first value
 * @b: pointer to second value
 *
 * Return: negative, zero, or positive per standard comparator rules
 */
static int cmp_ulong(void const *a, void const *b)
{
	unsigned long ua = *(unsigned long const *)a;
	unsigned long ub = *(unsigned long const *)b;

	return (ua < ub ? -1 : (ua > ub ? 1 : 0));
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
	unsigned long n, *arr;
	size_t cnt = 0, cap = 16, i;

	if (!s)
		return (NULL);
	list = malloc(sizeof(*list));
	if (!list)
		return (NULL);
	list_init(list);

	n = strtoul(s, NULL, 10);
	arr = malloc(cap * sizeof(*arr));
	if (!arr || (n > 1 && !collect_factors(n, &arr, &cnt, &cap)))
	{
		free(arr);
		list_destroy(list, free);
		free(list);
		return (NULL);
	}

	qsort(arr, cnt, sizeof(*arr), cmp_ulong);
	for (i = 0; i < cnt; i++)
	{
		if (!add_factor(list, arr[i]))
		{
			free(arr);
			list_destroy(list, free);
			free(list);
			return (NULL);
		}
	}
	free(arr);
	return (list);
}
