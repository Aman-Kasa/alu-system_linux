#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "response.h"

/**
 * send_simple_response - sends a bare HTTP status line with no body
 * @client_fd: the connected client's socket file descriptor
 * @code: the HTTP status code (e.g. 200, 404)
 * @reason: the HTTP status reason phrase (e.g. "OK", "Not Found")
 */
void send_simple_response(int client_fd, int code, const char *reason)
{
	char response[256];

	snprintf(response, sizeof(response),
		 "HTTP/1.1 %d %s\r\n\r\n", code, reason);
	send(client_fd, response, strlen(response), 0);
}

/**
 * send_json_response - sends an HTTP response carrying a JSON body
 * @client_fd: the connected client's socket file descriptor
 * @code: the HTTP status code (e.g. 200, 201)
 * @reason: the HTTP status reason phrase (e.g. "OK", "Created")
 * @json_body: the JSON body to send
 */
void send_json_response(int client_fd, int code, const char *reason,
			 const char *json_body)
{
	char header[256];

	snprintf(header, sizeof(header),
		 "HTTP/1.1 %d %s\r\n"
		 "Content-Length: %zu\r\n"
		 "Content-Type: application/json\r\n\r\n",
		 code, reason, strlen(json_body));

	send(client_fd, header, strlen(header), 0);
	send(client_fd, json_body, strlen(json_body), 0);
}
