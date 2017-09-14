typedef struct _cursor {
    int x;
    int y;
} Cursor;

Cursor* createCursor(int boardWidth, int boardLength);
void moveCursor(Cursor* c, int xIncr, int yIncr, int boardWidth, int boardLength);
void printCursor(Cursor* c, bool isHidden, bool markedAsMine, int surroundingMines);