#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

/**
 * main - connects to a server and sends it a message
 * @argc: argument count
 * @argv: argument vector: program name, host, port, message
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on usage error or
 * connection failure
 */
int main(int argc, char *argv[])
{
	struct addrinfo hints;
	struct addrinfo *result;
	int sockfd;
	int status;

	if (argc < 4)
	{
		fprintf(stderr, "Usage: %s <host> <port> <message>\n",
			argv[0]);
		return (EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return (EXIT_FAILURE);
	}

	sockfd = socket(result->ai_family, result->ai_socktype,
			result->ai_protocol);
	if (sockfd == -1)
	{
		perror("socket");
		freeaddrinfo(result);
		return (EXIT_FAILURE);
	}

	if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
	{
		perror("connect");
		freeaddrinfo(result);
		close(sockfd);
		return (EXIT_FAILURE);
	}

	printf("Connected to %s:%s\n", argv[1], argv[2]);
	printf("Sending the message: \"%s\"\n", argv[3]);
	send(sockfd, argv[3], strlen(argv[3]), 0);
	printf("Sent\n");

	freeaddrinfo(result);
	close(sockfd);

	return (EXIT_SUCCESS);
}
