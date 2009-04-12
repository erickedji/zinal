
CC	= gcc
CFLAGS 	= -g -Wall

all	: $(OBJ)
	$(CC) equadiff_num.c ../pretty_io/pretty_io.o ../menu_generique/menug.o ../parser/parser.o ../open_file/open_file.o -lm -o equadiff_num

clean	:
	rm -f $(OBJ) equadiff_num
