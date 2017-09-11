#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tile.h"

Tile* createTile(int xCoord, int yCoord) {
    Tile* t = (Tile *) malloc(sizeof(Tile));
            t->isHidden = true;
            t->hasMine = false;
            t->markedAsMine = false;
            t->x = xCoord;
            t->y = yCoord;
}

int minesAroundTile(Tile* t) {
    return 0;
}

void printTile(Tile* t) {
    if (t->isHidden) {
        printf("▢ ");
    } else {
        
    }
}