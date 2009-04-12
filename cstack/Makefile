
CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o cstack.o

test	: $(OBJ)
	$(CC) $(OBJ) ../pretty_io/pretty_io.o -o cstack_test

test.o		: test.c cstack.h 
cstack.o	: cstack.c cstack.h 

clean	:
	rm -f $(OBJ) cstack_test

