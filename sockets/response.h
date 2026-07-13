#ifndef RESPONSE_H
#define RESPONSE_H

void send_simple_response(int client_fd, int code, const char *reason);
void send_json_response(int client_fd, int code, const char *reason,
			 const char *json_body);

#endif /* RESPONSE_H */
