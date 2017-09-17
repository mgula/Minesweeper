/*Author: Marcus Gula*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cursor.h"

/*Initialize the cursor to the approximate middle of the board.*/
Cursor* createCursor(int boardHeight, int boardWidth) {
    Cursor* c = (Cursor*) malloc(sizeof(Cursor));
    c->x = boardHeight/2;
    c->y = boardWidth/2;
    return c;
}

/*Move the cursor by the given x and y increase. main.c will only ever call this method with
xIncr and yIncr as 1, 0, or -1.*/
void moveCursor(Cursor* c, int xIncr, int yIncr, int boardLength, int boardWidth) {
    if (c->x + xIncr >= boardLength) {
        c->x = 0;
    } else if (c->x + xIncr < 0) {
        c->x = boardLength - 1;
    } else {
        c->x += xIncr;
    }
    if (c->y + yIncr >= boardWidth) {
        c->y = 0;
    } else if (c->y + yIncr < 0) {
        c->y = boardWidth - 1;
    } else {
        c->y += yIncr;
    }
}

void printCursor(Cursor* c, bool isHidden, bool markedAsMine, int surroundingMines) {
    if (markedAsMine) {
        printf("⊗ ");
    } else {
        printf("⇲ ");
    }
}