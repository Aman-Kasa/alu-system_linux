#include <stdio.h>
#include <unistd.h>
#include "socket_utils.h"

#define PORT 12345

/**
 * main - opens an IPv4/TCP socket, binds it to port 12345 on any
 * address, listens for traffic, and hangs indefinitely
 *
 * Return: always 0
 */
int main(void)
{
	int sockfd;

	sockfd = create_server_socket(PORT);
	printf("Server listening on port %d\n", PORT);

	while (1)
		pause();

	(void)sockfd;
	return (0);
}
