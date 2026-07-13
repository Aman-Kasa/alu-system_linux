#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_parser.h"

/**
 * parse_kv_list - splits a string of "key=value" pairs joined by a
 * separator into a kv_pair_t array
 * @str: the string to split (e.g. "a=1&b=2")
 * @sep: the separator between pairs (e.g. '&')
 * @pairs: destination array
 * @count: pointer to the number of pairs already stored, updated in place
 */
static void parse_kv_list(const char *str, char sep,
			   kv_pair_t *pairs, int *count)
{
	char buf[MAX_BODY];
	char *token;
	char *equal_sign;
	char *saveptr;

	if (str == NULL || str[0] == '\0')
		return;

	strncpy(buf, str, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';

	token = strtok_r(buf, (char[]){sep, '\0'}, &saveptr);
	while (token != NULL && *count < MAX_PAIRS)
	{
		equal_sign = strchr(token, '=');
		if (equal_sign != NULL)
		{
			*equal_sign = '\0';
			strncpy(pairs[*count].key, token, MAX_KEY - 1);
			pairs[*count].key[MAX_KEY - 1] = '\0';
			strncpy(pairs[*count].value, equal_sign + 1,
				MAX_VALUE - 1);
			pairs[*count].value[MAX_VALUE - 1] = '\0';
		}
		else
		{
			strncpy(pairs[*count].key, token, MAX_KEY - 1);
			pairs[*count].key[MAX_KEY - 1] = '\0';
			pairs[*count].value[0] = '\0';
		}
		(*count)++;
		token = strtok_r(NULL, (char[]){sep, '\0'}, &saveptr);
	}
}

/**
 * parse_request_line - parses the first line of an HTTP request
 * @line: the request line (e.g. "GET /path?a=1 HTTP/1.1")
 * @req: destination request struct
 */
static void parse_request_line(const char *line, http_request_t *req)
{
	char path_and_query[MAX_PATH + MAX_BODY];
	char *question_mark;

	req->method[0] = '\0';
	req->path[0] = '\0';
	req->version[0] = '\0';
	path_and_query[0] = '\0';

	sscanf(line, "%15s %767s %15s",
	       req->method, path_and_query, req->version);

	question_mark = strchr(path_and_query, '?');
	if (question_mark != NULL)
	{
		*question_mark = '\0';
		parse_kv_list(question_mark + 1, '&',
			      req->queries, &req->query_count);
	}

	strncpy(req->path, path_and_query, MAX_PATH - 1);
	req->path[MAX_PATH - 1] = '\0';
}

/**
 * parse_headers - parses the header lines of an HTTP request
 * @headers_block: the raw block of header lines, separated by "\r\n"
 * @req: destination request struct
 */
static void parse_headers(char *headers_block, http_request_t *req)
{
	char *line;
	char *saveptr;
	char *colon;
	char *value;

	line = strtok_r(headers_block, "\r\n", &saveptr);
	while (line != NULL && req->header_count < MAX_PAIRS)
	{
		colon = strchr(line, ':');
		if (colon != NULL)
		{
			*colon = '\0';
			value = colon + 1;
			while (*value == ' ')
				value++;

			strncpy(req->headers[req->header_count].key,
				line, MAX_KEY - 1);
			req->headers[req->header_count].key[MAX_KEY - 1]
				= '\0';
			strncpy(req->headers[req->header_count].value,
				value, MAX_VALUE - 1);
			req->headers[req->header_count].value[MAX_VALUE - 1]
				= '\0';
			req->header_count++;
		}
		line = strtok_r(NULL, "\r\n", &saveptr);
	}
}

/**
 * parse_request - parses a raw HTTP/1.1 request into a http_request_t
 * @raw: the raw request text as received from the client
 * @req: destination struct, filled in by this function
 */
void parse_request(const char *raw, http_request_t *req)
{
	const char *header_end;
	char headers_block[MAX_BODY];
	const char *first_line_end;
	char first_line[MAX_PATH + MAX_BODY];
	size_t first_line_len;
	size_t headers_len;

	memset(req, 0, sizeof(*req));
	req->content_length = -1;

	first_line_end = strstr(raw, "\r\n");
	if (first_line_end == NULL)
		return;

	first_line_len = (size_t)(first_line_end - raw);
	if (first_line_len >= sizeof(first_line))
		first_line_len = sizeof(first_line) - 1;
	memcpy(first_line, raw, first_line_len);
	first_line[first_line_len] = '\0';
	parse_request_line(first_line, req);

	header_end = strstr(raw, "\r\n\r\n");
	if (header_end == NULL)
		return;

	headers_len = (size_t)(header_end + 2 - (first_line_end + 2));
	if (headers_len >= sizeof(headers_block))
		headers_len = sizeof(headers_block) - 1;
	memcpy(headers_block, first_line_end + 2, headers_len);
	headers_block[headers_len] = '\0';
	parse_headers(headers_block, req);

	strncpy(req->body, header_end + 4, MAX_BODY - 1);
	req->body[MAX_BODY - 1] = '\0';

	if (get_header(req, "Content-Length") != NULL)
		req->content_length = atoi(get_header(req, "Content-Length"));

	if (req->body[0] != '\0')
		parse_kv_list(req->body, '&',
			      req->body_params, &req->body_param_count);
}

/**
 * get_header - looks up a header value by name (case-sensitive)
 * @req: the parsed request to search
 * @name: the header name to look up
 *
 * Return: the header value, or NULL if not found
 */
const char *get_header(const http_request_t *req, const char *name)
{
	int i;

	for (i = 0; i < req->header_count; i++)
	{
		if (strcmp(req->headers[i].key, name) == 0)
			return (req->headers[i].value);
	}
	return (NULL);
}
