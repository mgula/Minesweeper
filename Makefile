all: run

tile.o: tile.h tile.c
	gcc -c -std=c99 tile.c

cursor.o: cursor.h cursor.c
	gcc -c -std=c99 cursor.c

main.o: tile.h cursor.h main.c
	gcc -c -std=c99 main.c

minesweeper: main.o tile.o cursor.o
	gcc -o minesweeper main.o tile.o cursor.o

run : minesweeper
	./minesweeper

clean :
	rm -f *.exe *~ *.o
