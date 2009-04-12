CC	= gcc
CFLAGS	= -g -Wall
OBJ	= menug.o test.o

test:	$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) ../pretty_io/pretty_io.o -o test_menug

menug.o:	menug.c menug.h ../pretty_io/pretty_io.h
test.o:		test.c menug.h ../pretty_io/pretty_io.h

clean:
	rm -f $(OBJ) test_menug

