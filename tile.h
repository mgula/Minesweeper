/*Author: Marcus Gula
The board will be composed of pointers to tiles. Tiles have an x and y coordinate, a certain
number of surrounding mines (0 - 8 possible, never seen an 8 though) and some additional booleans.*/
typedef struct _tile {
    int x;
    int y;
    int surroundingMines;
    bool hasMine;
    bool isHidden;
    bool markedAsMine;
} Tile;

Tile* createTile(int xCoord, int yCoord);
void printTile(Tile* t, bool lose, bool win);
void printTrueTile(Tile* t);