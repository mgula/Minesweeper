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
void printTile(Tile* t, bool lose) {
    if (t->hasMine && lose) {
        printf("X ");
        return;
    }
    
    if (t->markedAsMine) {
        printf("x ");
        return;
    }
    
    if (t->isHidden) {
        printf("▢ ");
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