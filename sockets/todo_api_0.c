#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_utils.h"
#include "http_parser.h"
#include "response.h"

#define PORT 8080
#define BUFFER_SIZE 8192

/**
 * handle_client - handles one HTTP client connection
 * @client_fd: client socket descriptor
 * @client_addr: client address information
 */
void handle_client(int client_fd, struct sockaddr_in *client_addr)
{
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	http_request_t req;

	printf("Client connected: %s\n",
	       inet_ntoa(client_addr->sin_addr));

	bytes_read = recv(client_fd, buffer,
			  sizeof(buffer) - 1, 0);

	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';

		printf("Raw request: \"%s\"\n", buffer);

		parse_request(buffer, &req);

		printf("Method: %s\n", req.method);
		printf("Path: %s\n", req.path);
		printf("Version: %s\n", req.version);

		send_simple_response(client_fd, 200, "OK");
	}

	close(client_fd);
}
/**
 * main - starts HTTP server on port 8080
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

	sockfd = create_server_socket(PORT);
	printf("Server listening on port %d\n", PORT);

	while (1)
	{
		client_len = sizeof(client_addr);

		client_fd = accept(sockfd,
				   (struct sockaddr *)&client_addr,
				   &client_len);

		if (client_fd == -1)
			continue;

		handle_client(client_fd, &client_addr);
	}

	return (0);
}
