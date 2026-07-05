CC       = gcc
CFLAGS   = -g3 -Wall -Werror -Wextra -pedantic
FILENAME = strace

all: strace_0 strace_1 strace_2 strace_3 strace_4 strace_5 strace_6 strace_7 strace_8

strace_0: 0-$(FILENAME).c
	$(CC) $(CFLAGS) 0-$(FILENAME).c -o strace_0

strace_1: 1-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 1-$(FILENAME).c -o strace_1

strace_2: 2-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 2-$(FILENAME).c -o strace_2

strace_3: 3-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 3-$(FILENAME).c -o strace_3

strace_4: 4-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 4-$(FILENAME).c -o strace_4

strace_5: 5-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 5-$(FILENAME).c -o strace_5

strace_6: 6-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 6-$(FILENAME).c -o strace_6

strace_7: 7-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 7-$(FILENAME).c -o strace_7

strace_8: 8-$(FILENAME).c syscalls.h
	$(CC) $(CFLAGS) 8-$(FILENAME).c -o strace_8

clean:
	rm -f strace_0 strace_1 strace_2 strace_3 strace_4 strace_5 strace_6 strace_7 strace_8

.PHONY: all clean
