/*Author: Marcus Gula
A cursor is effectively just an x and y coordinate.*/
typedef struct _cursor {
    int x;
    int y;
} Cursor;

Cursor* createCursor(int boardLength, int boardWidth);
void moveCursor(Cursor* c, int xIncr, int yIncr, int boardWidth, int boardLength);
void printCursor(Cursor* c, bool isHidden, bool markedAsMine, int surroundingMines, bool lose, bool win, bool unicode);