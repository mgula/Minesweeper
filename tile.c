/*Author: Marcus Gula*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tile.h"

/*Allocate a new tile on the heap.*/
Tile* createTile(int xCoord, int yCoord) {
    Tile* t = (Tile*) malloc(sizeof(Tile*));
    t->isHidden = true;
    t->hasMine = false;
    t->markedAsMine = false;
    t->surroundingMines = 0;
    t->x = xCoord;
    t->y = yCoord;
    return t;
}

/*Regular printing of a tile.*/
void printTile(Tile* t, bool lose, bool win, bool unicode) {
    if (lose) {
        if (t->hasMine) {
            if (t->markedAsMine) {
                if (unicode) {
                    printf("⚐ ");
                } else {
                    printf("x ");
                }
            } else {
                if (unicode) {
                    printf("☠ ");
                } else {
                    printf("X ");
                }
            }
            return;
        }
    } else if (win) {
        if (t->hasMine) {
            if (unicode) {
                printf("⚐ ");
            } else {
                printf("x ");
            }
            return;
        }
    }
    
    if (t->markedAsMine) {
        printf("x ");
        return;
    }
    
    if (t->isHidden) {
        if (unicode) {
            printf("▢ ");
        } else {
            printf("* ");
        }
    } else {
        if (t->surroundingMines == 0) {
            printf("  ");
        } else {
            printf("%d ", t->surroundingMines);
        }
    }
}

/*Debug printing.*/
void printTrueTile(Tile* t) {
    if (t->hasMine) {
        printf("X ");
    } else {
        if (t->surroundingMines == 0) {
            printf("  ");
        } else {
            printf("%d ", t->surroundingMines);
        }
    }
}