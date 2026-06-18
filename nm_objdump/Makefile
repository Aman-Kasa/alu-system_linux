CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
OBJ = main.o processor.o utils.o

hnm: $(OBJ)
	$(CC) $(OBJ) -o hnm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) hnm
