

CC	= gcc
CFLAGS 	= -g -Wall
OBJ	= test.o parser.o

test	: $(OBJ)
	$(CC) $(OBJ) ../pretty_io/pretty_io.o ../menu_generique/menug.o -lm -o parser_test

test.o		: test.c parser.h 
parser.o	: parser.c parser.h 

clean	:
	rm -f $(OBJ) parser_test

