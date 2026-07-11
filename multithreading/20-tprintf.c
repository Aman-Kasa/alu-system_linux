#include "multithreading.h"
#include <stdio.h>

static pthread_mutex_t mutex;

/**
 * init_mutex - Constructor: initialise the mutex before main()
 */
__attribute__((constructor))
void init_mutex(void)
{
	pthread_mutex_init(&mutex, NULL);
}

/**
 * destroy_mutex - Destructor: destroy the mutex after main()
 */
__attribute__((destructor))
void destroy_mutex(void)
{
	pthread_mutex_destroy(&mutex);
}

/**
 * tprintf - Thread‑safe formatted printing with thread ID prefix
 * @format: Format string
 *
 * Return: Number of characters printed
 */
int tprintf(char const *format, ...)
{
	va_list args;
	int ret;

	pthread_mutex_lock(&mutex);

	va_start(args, format);
	ret = printf("[%lu] ", pthread_self());
	ret += vprintf(format, args);
	va_end(args);

	pthread_mutex_unlock(&mutex);
	return (ret);
}
