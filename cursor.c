#include <stdlib.h>

#include "cursor.h"

/*Initialize the cursor to the approximate middle of the board.*/
Cursor* createCursor(int boardWidth, int boardLength) {
    Cursor* c = (Cursor*) malloc(sizeof(Cursor));
    c->x = boardWidth/2;
    c->y = boardLength/2;
    return c;
}

void moveCursor(Cursor* c, int xIncr, int yIncr, int boardWidth, int boardLength) {
    if (c->x + xIncr >= boardWidth) {
        c->x = 0;
    } else if (c->x + xIncr < 0) {
        c->x = boardWidth - 1;
    } else {
        c->x += xIncr;
    }
    if (c->y + yIncr >= boardLength) {
        c->y = 0;
    } else if (c->y + yIncr < 0) {
        c->y = boardLength - 1;
    } else {
        c->y += yIncr;
    }
}