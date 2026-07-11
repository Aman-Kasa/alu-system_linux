#include "multithreading.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * thread_entry - Entry point for a new thread
 * @arg: Address of a string to print
 *
 * Return: NULL after printing the string
 */
void *thread_entry(void *arg)
{
	printf("%s\n", (char *)arg);
	pthread_exit(NULL);
}
