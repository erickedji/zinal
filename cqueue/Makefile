
CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o cqueue.o

test	: $(OBJ)
	$(CC) $(OBJ) ../pretty_io/pretty_io.o -o cqueue_test

test.o		: test.c cqueue.h 
cqueue.o	: cqueue.c cqueue.h 

clean	:
	rm -f $(OBJ) cqueue_test

