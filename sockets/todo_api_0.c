#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_utils.h"
#include "http_parser.h"
#include "response.h"

#define PORT 8080
#define BUFFER_SIZE 8192

/**
 * main - opens an IPv4/TCP socket on port 8080, and for each connection
 * prints the client IP, the raw HTTP request, and a breakdown of its
 * first line (method, path, version), then replies 200 OK
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
	http_request_t req;

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
			printf("Raw request: \"%s\"\n", buffer);

			parse_request(buffer, &req);
			printf("Method: %s\n", req.method);
			printf("Path: %s\n", req.path);
			printf("Version: %s\n", req.version);

			send_simple_response(client_fd, 200, "OK");
		}

		close(client_fd);
	}

	return (0);
}
