#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tile.h"

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

int minesAroundTile(Tile* t) {
    return 0;
}

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