
CC	= gcc
CFLAGS 	= -g -Wall

all	: lagrange.c
	$(CC) lagrange.c ../pretty_io/pretty_io.o ../menu_generique/menug.o ../open_file/open_file.o -lm -o lagrange

clean	:
	rm -f $(OBJ) lagrange
