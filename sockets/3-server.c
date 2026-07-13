#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_utils.h"

#define PORT 12345
#define BUFFER_SIZE 4096

/**
 * main - opens an IPv4/TCP socket, binds it to port 12345 on any
 * address, and repeatedly accepts connections, printing the client's
 * IP address and any message it sends before closing the connection
 *
 * Return: always 0
 */
int main(void)
{
	int sockfd;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

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

		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			printf("Message received: \"%s\"\n", buffer);
		}

		close(client_fd);
	}

	return (0);
}
