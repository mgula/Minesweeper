#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "cursor.h"

/*Initialize the cursor to the approximate middle of the board.*/
Cursor* createCursor(int boardLength, int boardWidth) {
    Cursor* c = (Cursor*) malloc(sizeof(Cursor));
    c->x = boardLength/2;
    c->y = boardWidth/2;
    return c;
}

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
        /*i like the below concept but need to find better unicode characters*/
        /*if (isHidden) {
            printf("⇲ ");
        } else {
            if (surroundingMines == 0) {
                printf("⇲ ");
            } else if (surroundingMines == 1) {
                printf("①");
            } else if (surroundingMines == 2) {
                printf("②");
            } else if (surroundingMines == 3) {
                printf("③");
            } else if (surroundingMines == 4) {
                printf("④");
            } else if (surroundingMines == 5) {
                printf("⑤");
            } else if (surroundingMines == 6) {
                printf("⑥");
            } else if (surroundingMines == 7) {
                printf("⑦");
            } else if (surroundingMines == 8) {
                printf("⑧");
            }
        }*/
    }
}