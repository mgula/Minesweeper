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

typedef enum {NONE, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, MARK_MINE, ACTIVATE, PRINT, DEBUG, QUIT} Action;

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

bool debug = false;

/*Methods for operations on the board.*/
void printBoard(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (i == cursor->x && j == cursor->y) {
                printCursor(cursor, board[i][j]->isHidden, board[i][j]->markedAsMine, board[i][j]->surroundingMines);
            } else {
                if (debug) {
                    printTrueTile(board[i][j]);
                } else {
                    printTile(board[i][j], false);
                }
            }
        }
        printf("\n");
    }
}

void printLosingBoard(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            printTile(board[i][j], true);
        }
        printf("\n");
    }
}

void markMine(Tile* board[][width]) {
    board[cursor->x][cursor->y]->markedAsMine = !board[cursor->x][cursor->y]->markedAsMine;
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
    
    /*place mines*/
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < numMines; k++) {
                if (board[i][j]->x == mineX[k] && board[i][j]->y == mineY[k]) {
                    board[i][j]->hasMine = true;
                    board[i][j]->surroundingMines = -1;
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
    /*do nothing if current tile is marked as mine*/
    if (board[tileX][tileY]->markedAsMine) {
        return;
    }
    /*if tile has mine, game is over*/
    if (board[tileX][tileY]->hasMine && initialCall) {
        lose = true;
        printLosingBoard(board);
        return;
    }
    /*recursively check around tiles with 0 surrounding mines*/
    if (board[tileX][tileY]->isHidden && board[tileX][tileY]->surroundingMines == 0 || firstMove) {
        if (firstMove) {
            firstMove = false;
        }
        board[tileX][tileY]->isHidden = false;
        /*four corner cases*/
        if (tileX == 0 && tileY == 0) {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
        } else if (tileX == length - 1 && tileY == 0) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);    
        } else if (tileX == 0 && tileY == width - 1) {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX, tileY - 1, false);   
        } else if (tileX == length - 1 && tileY == width - 1) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY - 1, false);   
        } 
        /*four edge cases*/
        else if (tileX == 0 && tileY != 0 && tileY != width - 1) {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false);
        } else if (tileY == 0 && tileX != 0 && tileX != length - 1) {
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);    
        } else if (tileX == length - 1 && tileY != 0 && tileY != width - 1) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false);    
        } else if (tileY == width - 1 && tileX != 0 && tileX != length - 1) {
            activateTile(board, tileX, tileY - 1, false);
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);    
        }
        /*everything else*/
        else {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false);
        }
    } else if (board[tileX][tileY]->isHidden && !board[tileX][tileY]->hasMine) {
        board[tileX][tileY]->isHidden = false;
        return;
    } else {
        return;
    }
    return;
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
        case 'j':
            currentAction = DEBUG;
            break;
        default:
            printf("Command not recognized. Press 'h' for a list of commands.\n");
            break;
    }
}

void evaluateInput(Tile* board[][width]) {
    switch (currentAction) {
        case MOVE_LEFT:
            moveCursor(cursor, 0, -1, length, width);
            break;
        case MOVE_DOWN:
            moveCursor(cursor, 1, 0, length, width);
            break;
        case MOVE_RIGHT:
            moveCursor(cursor, 0, 1, length, width);
            break;
        case MOVE_UP:
            moveCursor(cursor, -1, 0, length, width);
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
                setMines(board, cursor->x, cursor->y);
                calculateSurroudingMines(board);
            }
            activateTile(board, cursor->x, cursor->y, true);
            break;
        case DEBUG:
            debug = !debug;
            break;
        default:
            break;
    }
    currentAction = NONE;
}

void gameStateCheck(Tile* board[][width]) {
    bool allUncovered = true;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (!board[i][j]->hasMine && board[i][j]->isHidden) {
                allUncovered = false;
            }
        }
    }
    if (allUncovered) {
        win = true;
    }
}

void revertToStartingState(Tile* board[][width]) {
    win = false;
    lose = false;
    
    firstMove = true;
    
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            free(board[i][j]);
        }
    }
    free(cursor);
    
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = createTile(i, j);
        }
    }
    cursor = createCursor(length, width);
}

void promptNextAction() {
    printf("Play again? (y/n)\n");
    char input;
    
    bool resolved = false;
    
    while (!resolved) {
        if (scanf(" %c", &input) != -1) {
            if (input == 'n' || input == 'N') {
                play = false;
                resolved = true;
            } else if (input == 'y' || input == 'Y') {
                resolved = true;
            } else {
                printf("Enter y or n.\n");
            }
        } else {
            printf("Invalid command.\n");
        }
    }
}

int main (int argc, char* argv[]) {
    /*Set difficulty, and length and width of board.*/
    if (argc != 3) {
        /*For now, default to medium.*/
        length = width = MEDIUM;
        difficulty = INTERMEDIATE;
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
    cursor = createCursor(length, width);
    
    /*seed random number generator*/
    srand(time(NULL)); 
    
    while (play) {
        /*Print the board.*/
        printBoard(board);
        
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readInput();
            evaluateInput(board);
            gameStateCheck(board);
        } else {
            printf("Invalid command.\n");
        }
        
        /*Evaluate winning and losing conditions.*/
        if (win || lose) {
            if (win) {
                printf("Congratulations, you won!\n");
            }
            if (lose) {
                printf("Oops, a mine blew up.\n");
            }
            revertToStartingState(board);
            promptNextAction();
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