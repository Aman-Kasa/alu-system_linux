#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

/**
 * create_connection - creates and connects a socket
 * @result: address information
 *
 * Return: socket file descriptor on success, -1 on failure
 */
int create_connection(struct addrinfo *result)
{
	int sockfd;

	sockfd = socket(result->ai_family,
			result->ai_socktype,
			result->ai_protocol);
	if (sockfd == -1)
	{
		perror("socket");
		return (-1);
	}

	if (connect(sockfd, result->ai_addr,
		    result->ai_addrlen) == -1)
	{
		perror("connect");
		close(sockfd);
		return (-1);
	}

	return (sockfd);
}

/**
 * main - connects to a server given a host and port
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result;
	int sockfd;
	int status;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n",
			gai_strerror(status));
		return (EXIT_FAILURE);
	}

	sockfd = create_connection(result);
	if (sockfd == -1)
	{
		freeaddrinfo(result);
		return (EXIT_FAILURE);
	}

	printf("Connected to %s:%s\n", argv[1], argv[2]);

	freeaddrinfo(result);
	close(sockfd);

	return (EXIT_SUCCESS);
}
