# Makefile
CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
TARGET = hnm

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)
