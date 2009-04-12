
CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o cbtree.o

test	: $(OBJ)
	$(CC) $(OBJ) ../pretty_io/pretty_io.o -o cbtree_test

test.o		: test.c cbtree.h 
cbtree.o	: cbtree.c cbtree.h 

clean	:
	rm -f $(OBJ) cbtree_test
