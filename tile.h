typedef struct _tile {
    int x;
    int y;
    int surroundingMines;
    bool hasMine;
    bool isHidden;
    bool markedAsMine;
} Tile;

void printTile(Tile *t);