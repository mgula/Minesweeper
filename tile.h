typedef struct _tile {
    int x;
    int y;
    int surroundingMines;
    bool hasMine;
    bool isHidden;
    bool markedAsMine;
} Tile;

Tile* createTile(int xCoord, int yCoord);
void printTile(Tile* t, bool lose);
void printTrueTile(Tile* t);