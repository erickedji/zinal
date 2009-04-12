CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o clist.o

test	: $(OBJ)
	$(CC) $(OBJ) -o test

test.o	: test.c clist.h
clist.o	: clist.c clist.h

clean	:
	rm -f $(OBJ) test

