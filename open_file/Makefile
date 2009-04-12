
CC	= gcc
CFLAGS	= -g -Wall
OBJ	= open_file.o test.o

test:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) ../pretty_io/pretty_io.o -o open_file_test

open_file.o:	open_file.c open_file.h
test.o:		test.c open_file.h

clean:
	rm -f $(OBJ) open_file_test

