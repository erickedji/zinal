
CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o generic.o

test	: $(OBJ)
	$(CC) $(OBJ) ../pretty_io/pretty_io.o -o generic_test

test.o		: test.c generic.h 
generic.o	: generic.c generic.h 

clean	:
	rm -f $(OBJ) generic_test
