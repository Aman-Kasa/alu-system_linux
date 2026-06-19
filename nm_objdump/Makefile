CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
OBJ = main.o processor.o utils.o
OBJ_OD = hobjdump_main.o objdump64.o objdump32.o objdump_utils.o

hnm: $(OBJ)
	$(CC) $(OBJ) -o hnm

hobjdump: $(OBJ_OD)
	$(CC) $(OBJ_OD) -o hobjdump

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OBJ_OD) hnm hobjdump
