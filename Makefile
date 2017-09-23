CC = gcc
FLAGS = -std=c99 -w

all: run

tile.o: tile.h tile.c
	$(CC) -c $(FLAGS) tile.c

cursor.o: cursor.h cursor.c
	$(CC) -c $(FLAGS) cursor.c

main.o: tile.h cursor.h main.c
	$(CC) -c $(FLAGS) main.c

minesweeper: main.o tile.o cursor.o
	$(CC) -o minesweeper main.o tile.o cursor.o

run : minesweeper
	./minesweeper

clean :
	rm -f *~ *.o minesweeper
