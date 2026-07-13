#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_utils.h"

/**
 * create_server_socket - creates, binds and starts listening on a
 * IPv4/TCP socket
 * @port: the port to listen on (host byte order)
 *
 * Return: the listening socket file descriptor, or exits on failure
 */
int create_server_socket(int port)
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
	addr.sin_port = htons((unsigned short)port);

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
