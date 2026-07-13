#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define MAX_PAIRS 32
#define MAX_METHOD 16
#define MAX_PATH 512
#define MAX_VERSION 16
#define MAX_KEY 256
#define MAX_VALUE 1024
#define MAX_BODY 8192

/**
 * struct kv_pair - a generic key/value pair
 * @key: the key
 * @value: the value
 */
typedef struct kv_pair
{
	char key[MAX_KEY];
	char value[MAX_VALUE];
} kv_pair_t;

/**
 * struct http_request - a parsed HTTP/1.1 request
 * @method: the HTTP method (GET, POST, ...)
 * @path: the request path (without the query string)
 * @version: the HTTP version (e.g. HTTP/1.1)
 * @headers: array of parsed header key/value pairs
 * @header_count: number of headers parsed
 * @queries: array of parsed query string key/value pairs
 * @query_count: number of queries parsed
 * @body_params: array of parsed url-encoded body key/value pairs
 * @body_param_count: number of body params parsed
 * @body: raw body content
 * @content_length: value of the Content-Length header, or -1 if absent
 */
typedef struct http_request
{
	char method[MAX_METHOD];
	char path[MAX_PATH];
	char version[MAX_VERSION];
	kv_pair_t headers[MAX_PAIRS];
	int header_count;
	kv_pair_t queries[MAX_PAIRS];
	int query_count;
	kv_pair_t body_params[MAX_PAIRS];
	int body_param_count;
	char body[MAX_BODY];
	int content_length;
} http_request_t;

void parse_request(const char *raw, http_request_t *req);
const char *get_header(const http_request_t *req, const char *name);

#endif /* HTTP_PARSER_H */
