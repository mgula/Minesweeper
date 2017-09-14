#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "tile.h"
#include "cursor.h"

//to do: sudden death

#define EASY .05
#define INTERMEDIATE .10
#define HARD .20

#define SMALL 7
#define MEDIUM 9
#define LARGE 11

typedef enum {NONE, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, MARK_MINE, ACTIVATE, PRINT, QUIT} Action;

int length;
int width;

double difficulty;

bool play = true;
bool firstMove = true;

bool win = false;
bool lose = false;

char currentCommand = 'h';
Action currentAction = NONE;

Cursor* cursor;

/*Methods for operations on the board.*/
void printBoard(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (i == cursor->y && j == cursor->x) {
                printCursor(cursor, board[i][j]->markedAsMine, board[i][j]->surroundingMines);
            } else {
                printTile(board[i][j]);
            }
        }
        printf("\n");
    }
}

void markMine(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (i == cursor->y && j == cursor->x) {
                board[i][j]->markedAsMine = !board[i][j]->markedAsMine;
            }
        }
    }
}

bool inArray(int* arr, int x, int l) {
    int in = 0;
    for (int i = 0; i < l; i++) {
        if (arr[i] == x) {
            in++;
            if (in == 2) {
                return true;
            }
        }
    }
    return false;
}

void setMines(Tile* board[][width], int cursorX, int cursorY) {
    int numMines = (int) (length * width * difficulty);
    
    int* mineX = malloc(sizeof(int) * numMines);
    int* mineY = malloc(sizeof(int) * numMines);
    printf("cursor x = %d, cursor y = %d\n", cursorX, cursorY);
    printf("generating %d mines...\n", numMines);
    /*probably not the most optimal*/
    for (int i = 0; i < numMines; i++) {
        int x = rand() % length;
        int y = rand() % width;
        
        while (x == cursorX || inArray(mineX, x, length)) {
            x = rand() % length;
        }
        
        while (y == cursorY || inArray(mineY, y, width)) {
            y = rand() % width;
        }
        
        mineX[i] = x;
        mineY[i] = y;
    }
    printf("placing %d mines...\n", numMines);
    /*place mines*/
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < numMines; k++) {
                if (board[i][j]->x == mineX[k] && board[i][j]->y == mineY[k]) {
                    board[i][j]->hasMine = true;
                    board[i][j]->surroundingMines = -1;
                    printf("placed mine number %d at x = %d, y = %d\n", k, mineX[k], mineY[k]);
                }
            }
        }
    }
    
    free(mineX);
    free(mineY);
}

/*again, probably not the most optimal ...*/
void calculateSurroudingMines(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j]->hasMine) {
                continue;
            }
            /*four corner cases - only 3 surrounding tiles*/
            if (i == 0 && j == 0) {
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (i == length - 1 && j == 0) {
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (i == 0 && j == width - 1) {
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (i == length - 1 && j == width - 1) {
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            }
            /*four edge cases - only 5 surrounding tiles*/
            else if (i == 0 && j != 0 && j != width - 1) {
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (j == 0 && i != 0 && i != length - 1) {
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (i == length - 1 && j != 0 && j != width - 1) {
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            } else if (j == width - 1 && i != 0 && i != length - 1) {
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            }
            /*everything else - 8 surrounding tiles*/
            else {
                if (board[i + 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i - 1][j]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
                if (board[i + 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                } 
                if (board[i + 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                } 
                if (board[i - 1][j - 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                } 
                if (board[i - 1][j + 1]->hasMine) {
                    board[i][j]->surroundingMines += 1;
                }
            }
        }
    }
}

void activateTile(Tile* board[][width], int tileX, int tileY, bool initialCall) {
    printf("in activateTile\n");
    if (board[tileY][tileX]->markedAsMine) {
        return;
    }
    /*if tile has mine, game is over*/
    if (board[tileY][tileX]->hasMine && initialCall) {
        printf("a mine was activated :(\n");
        lose = true;
        return;
    }
    
    if (board[tileY][tileX]->isHidden && board[tileY][tileX]->surroundingMines == 0) {
        board[tileY][tileX]->isHidden = false;
        printf("found a 0 tile: tileX = %d, tileY = %d\n", tileX, tileY);
        /*consider edge + corner case here*/
        activateTile(board, tileX + 1, tileY, false);
        activateTile(board, tileX - 1, tileY, false);
        activateTile(board, tileX, tileY + 1, false);
        activateTile(board, tileX, tileY - 1, false);
    } else if (board[tileY][tileX]->isHidden) {
        board[tileY][tileX]->isHidden = false;
        return;
    } else {
        return;
    }
}

/*methods for handling user input.*/
char* getDifficultyString() {
    if (difficulty == EASY) {
        return "easy";
    } else if (difficulty == INTERMEDIATE) {
        return "intermediate";
    } else if (difficulty == HARD) {
        return "hard";
    }
}

void printControls() {
    printf("w = move cursor up\n");
    printf("a = move cursor left\n");
    printf("s = move cursor down\n");
    printf("d = move cursor right\n");
    printf("q = quit\n");
    printf("m = mark current tile as mine\n");
    printf("n = uncover current tile\n");
}

void readInput() {
    switch (currentCommand) {
        case 'a':
            currentAction = MOVE_LEFT;
            break;
        case 's':
            currentAction = MOVE_DOWN;
            break;
        case 'd':
            currentAction = MOVE_RIGHT;
            break;
        case 'w':
            currentAction = MOVE_UP;
            break;
        case 'q':
            currentAction = QUIT;
            break;
        case 'm':
            currentAction = MARK_MINE;
            break;
        case 'h':
            currentAction = PRINT;
            break;
        case 'n':
            currentAction = ACTIVATE;
            break;
        default:
            printf("Command not recognized. Press 'h' for a list of commands.\n");
            break;
    }
}

void evaluateInput(Tile* board[][width]) {
    switch (currentAction) {
        case MOVE_LEFT:
            moveCursor(cursor, -1, 0, width, length);
            break;
        case MOVE_DOWN:
            moveCursor(cursor, 0, 1, width, length);
            break;
        case MOVE_RIGHT:
            moveCursor(cursor, 1, 0, width, length);
            break;
        case MOVE_UP:
            moveCursor(cursor, 0, -1, width, length);
            break;
        case QUIT:
            play = false;
            break;
        case MARK_MINE:
            markMine(board);
            break;
        case PRINT:
            printControls();
            break;
        case ACTIVATE:
            if (firstMove) {
                firstMove = false;
                setMines(board, cursor->x, cursor->y);
                calculateSurroudingMines(board);
            }
            activateTile(board, cursor->x, cursor->y, true);
            break;
        default:
            break;
    }
    currentAction = NONE;
}

void gameStateCheck(Tile* board[][width]) {
    
}

int main (int argc, char* argv[]) {
    /*Set difficulty, and length and width of board.*/
    if (argc != 3) {
        /*For now, default to medium.*/
        //length = width = MEDIUM;
        //difficulty = INTERMEDIATE;
        length = 10;
        width = 25;
        difficulty = HARD;
    } else if (argc == 3) {
        if (*argv[1] == 's' || *argv[1] == 'S') {
            length = width = SMALL;
        } else if (*argv[1] == 'm' || *argv[1] == 'M') {
            length = width = MEDIUM;
        } else if (*argv[1] == 'l' || *argv[1] == 'L') {
            length = width = LARGE;
        } else {
            /*default to medium if bad entry*/
            length = width = MEDIUM;
        }
        if (*argv[2] == 'e' || *argv[2] == 'E') {
            difficulty = EASY;
        } else if (*argv[2] == 'i' || *argv[2] == 'I') {
            difficulty = INTERMEDIATE;
        } else if (*argv[2] == 'h' || *argv[2] == 'H') {
            difficulty = HARD;
        } else {
            /*default to intermediate if bad entry*/
            difficulty = INTERMEDIATE;
        }
    } 
    printf("Initializing board with length = %d, width = %d, and difficulty = %s.\n", length, width, getDifficultyString());
    
    /*Initialize board with the set length and width, initially no mines.*/
    Tile* board[length][width];
    
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = createTile(i, j);
        }
    }
    
    /*Initialize cursor to approximate middle of board.*/
    cursor = createCursor(width, length);
    
    /*seed random number generator*/
    srand(time(NULL)); 
    
    while (play) {
        /*Print board.*/
        if (win) {
            printf("congratz\n");
            continue;
        }
        if (lose) {
            printf("oops\n");
            play = false;
            continue;
        } else {
            printBoard(board);
        }
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readInput();
            evaluateInput(board);
            gameStateCheck(board);
        } else {
            printf("Invalid command.\n");
        }
    }
    
    /*Free memory.*/
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            free(board[i][j]);
        }
    }
    free(cursor);
    
    printf("Program closed.\n");
    return 0;
}