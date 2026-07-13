#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	int opt;
	int client_fd;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	socklen_t client_len;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		       &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 128) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

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
