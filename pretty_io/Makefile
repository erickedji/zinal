
CC	= gcc
CFLAGS	= -g -Wall
OBJ	= pretty_io.o test.o

test:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test_pretty_io

pretty_io.o:	pretty_io.c pretty_io.h
test.o:		test.c pretty_io.h

clean:
	rm -f $(OBJ) test_pretty_io

