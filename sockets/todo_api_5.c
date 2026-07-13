#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_utils.h"
#include "http_parser.h"
#include "response.h"
#include "todo.h"

#define PORT 8080
#define BUFFER_SIZE 8192
#define JSON_BUF_SIZE 65536

/**
 * find_body_param - looks up a body parameter value by key
 * @req: the parsed request to search
 * @key: the body parameter key to look up
 *
 * Return: the value, or NULL if not found
 */
static const char *find_body_param(const http_request_t *req,
				    const char *key)
{
	int i;

	for (i = 0; i < req->body_param_count; i++)
	{
		if (strcmp(req->body_params[i].key, key) == 0)
			return (req->body_params[i].value);
	}
	return (NULL);
}

/**
 * handle_post_todos - handles POST /todos: creates a new todo
 * @client_fd: the connected client's socket file descriptor
 * @req: the parsed HTTP request
 * @client_ip: the connected client's IP address, for logging
 */
static void handle_post_todos(int client_fd, const http_request_t *req,
			       const char *client_ip)
{
	const char *title;
	const char *description;
	char json[MAX_TITLE + MAX_DESC + 64];
	int id;

	if (get_header(req, "Content-Length") == NULL)
	{
		printf("%s POST /todos -> 411 Length Required\n", client_ip);
		send_simple_response(client_fd, 411, "Length Required");
		return;
	}

	title = find_body_param(req, "title");
	description = find_body_param(req, "description");

	if (title == NULL || description == NULL)
	{
		printf("%s POST /todos -> 422 Unprocessable Entity\n",
		       client_ip);
		send_simple_response(client_fd, 422, "Unprocessable Entity");
		return;
	}

	id = add_todo(title, description);
	todo_to_json(find_todo(id), json, sizeof(json));

	printf("%s POST /todos -> 201 Created\n", client_ip);
	send_json_response(client_fd, 201, "Created", json);
}

/**
 * handle_get_todos - handles GET /todos: retrieves every todo
 * @client_fd: the connected client's socket file descriptor
 * @client_ip: the connected client's IP address, for logging
 */
static void handle_get_todos(int client_fd, const char *client_ip)
{
	static char json[JSON_BUF_SIZE];

	todos_to_json(json, sizeof(json));

	printf("%s GET /todos -> 200 OK\n", client_ip);
	send_json_response(client_fd, 200, "OK", json);
}

/**
 * route_request - dispatches a parsed request to the right handler,
 * or replies 404 Not Found for unrecognized routes/methods
 * @client_fd: the connected client's socket file descriptor
 * @req: the parsed HTTP request
 * @client_ip: the connected client's IP address, for logging
 */
static void route_request(int client_fd, const http_request_t *req,
			   const char *client_ip)
{
	if (strcmp(req->path, "/todos") == 0)
	{
		if (strcmp(req->method, "POST") == 0)
		{
			handle_post_todos(client_fd, req, client_ip);
			return;
		}
		if (strcmp(req->method, "GET") == 0)
		{
			handle_get_todos(client_fd, client_ip);
			return;
		}
	}

	printf("%s %s %s -> 404 Not Found\n",
	       client_ip, req->method, req->path);
	send_simple_response(client_fd, 404, "Not Found");
}

/**
 * main - opens an IPv4/TCP socket on port 8080 and implements the
 * POST /todos and GET /todos routes of the TODO REST API, replying
 * 404 for anything else
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
	char client_ip[INET_ADDRSTRLEN];

	sockfd = create_server_socket(PORT);
	printf("Server listening on port %d\n", PORT);

	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
				    &client_len);
		if (client_fd == -1)
			continue;

		strncpy(client_ip, inet_ntoa(client_addr.sin_addr),
			sizeof(client_ip) - 1);
		client_ip[sizeof(client_ip) - 1] = '\0';

		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			parse_request(buffer, &req);
			route_request(client_fd, &req, client_ip);
		}

		close(client_fd);
	}

	return (0);
}
