typedef struct _cursor {
    int x;
    int y;
} Cursor;

void moveCursor(Cursor* c, int xIncr, int yIncr, int boardWidth, int boardLength);