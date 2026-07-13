#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_utils.h"

#define PORT 12345

/**
 * main - opens an IPv4/TCP socket, binds it to port 12345 on any
 * address, and repeatedly accepts incoming connections, printing the
 * connected client's IP address before closing each connection
 *
 * Return: always 0
 */
int main(void)
{
	int sockfd;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;

	sockfd = create_server_socket(PORT);
	printf("Server listening on port %d\n", PORT);

	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
				    &client_len);
		if (client_fd == -1)
			continue;

		printf("Client connected: %s\n",
		       inet_ntoa(client_addr.sin_addr));

		close(client_fd);
	}

	return (0);
}
