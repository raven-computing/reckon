# Second Make sample

SRC = main.c util.c
OBJ = $(SRC:.c=.o)
clean:
	rm -f $(OBJ)
