CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

COMMON_SRC = socket_utils.c http_parser.c response.c todo.c

all: 0-server 1-server 2-client 3-server 3-client \
	todo_api_0 todo_api_1 todo_api_2 todo_api_3 todo_api_4 todo_api_5

0-server: 0-server.c socket_utils.c
	$(CC) $(CFLAGS) -o $@ 0-server.c socket_utils.c

1-server: 1-server.c socket_utils.c
	$(CC) $(CFLAGS) -o $@ 1-server.c socket_utils.c

2-client: 2-client.c
	$(CC) $(CFLAGS) -o $@ 2-client.c

3-server: 3-server.c socket_utils.c
	$(CC) $(CFLAGS) -o $@ 3-server.c socket_utils.c

3-client: 3-client.c
	$(CC) $(CFLAGS) -o $@ 3-client.c

todo_api_0: todo_api_0.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_0.c $(COMMON_SRC)

todo_api_1: todo_api_1.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_1.c $(COMMON_SRC)

todo_api_2: todo_api_2.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_2.c $(COMMON_SRC)

todo_api_3: todo_api_3.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_3.c $(COMMON_SRC)

todo_api_4: todo_api_4.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_4.c $(COMMON_SRC)

todo_api_5: todo_api_5.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ todo_api_5.c $(COMMON_SRC)

clean:
	rm -f 0-server 1-server 2-client 3-server 3-client \
		todo_api_0 todo_api_1 todo_api_2 todo_api_3 todo_api_4 todo_api_5

.PHONY: all clean
