# to run the program, enter "make all" or "make run" on the command line.

# if the board is printing strangely (you'll know), try building without 
# unicode characters using "make ascii". you can also toggle betweena unicode/ascii in game.

CC = gcc
FLAGS = -std=c99 -w

all: clean run

ascii : clean run_ascii

tile.o: tile.h tile.c
	$(CC) -c $(FLAGS) tile.c

cursor.o: cursor.h cursor.c
	$(CC) -c $(FLAGS) cursor.c

main.o: tile.h cursor.h main.c
	$(CC) -c $(FLAGS) -DCHARSTYLE=true main.c

main_ascii.o: tile.h cursor.h main.c
	$(CC) -c $(FLAGS) -DCHARSTYLE=false main.c

minesweeper: main.o tile.o cursor.o
	$(CC) -o minesweeper main.o tile.o cursor.o

minesweeper_ascii: main_ascii.o tile.o cursor.o
	$(CC) -o minesweeper_ascii main.o tile.o cursor.o

# to compile with command line arguments, add your arguments after ./minesweeper,
# like so: ./minesweeper <size> <difficulty>
# s = small, m = medium, l = large
# e = easy, i = intermediate, h = hard
# for example, ./minesweeper l h would start a game with a large board and hard difficulty.
run : minesweeper
	./minesweeper

run_ascii : minesweeper_ascii
	./minesweeper_ascii

clean :
	rm -f *~ *.o minesweeper minesweeper_ascii
