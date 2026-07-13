#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 4096

/**
 * create_server_socket - creates and prepares server socket
 *
 * Return: socket file descriptor
 */
int create_server_socket(void)
{
	int sockfd;
	int opt;
	struct sockaddr_in addr;

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

	return (sockfd);
}

/**
 * handle_client - accepts and reads client messages
 * @client_fd: client socket descriptor
 * @client_addr: client address
 */
void handle_client(int client_fd, struct sockaddr_in *client_addr)
{
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;

	printf("Client connected: %s\n",
	       inet_ntoa(client_addr->sin_addr));

	bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		printf("Message received: \"%s\"\n", buffer);
	}

	close(client_fd);
}

/**
 * main - starts server and handles connections
 *
 * Return: always 0
 */
int main(void)
{
	int sockfd;
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;

	setbuf(stdout, NULL);

	sockfd = create_server_socket();
	printf("Server listening on port %d\n", PORT);

	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
				    &client_len);
		if (client_fd == -1)
			continue;
		handle_client(client_fd, &client_addr);
	}

	return (0);
}
