/*Command line version of the game Minesweeper. Some notes: Board doesn't 
have it's own header + c file because it's difficult to pass the length and 
width of the board to methods in other files, especially since both of those
dimensions are variable (the player decides on the command line or at runtime).

To me, it seemed the simplest solution was to make length and width global in 
this file so all methods have easy access to them.

Controls:
    q - quit
    w - move cursor up
    a - move cursor left
    s - move cursor down
    d - move cursor right
    n - uncover tile
    m - mark tile as mine*/
    
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "tile.h"
#include "cursor.h"

//to do: sudden death

#define MAX_RANGE 50
#define BUFF_SIZE 50

#define EASY .05
#define INTERMEDIATE .10
#define HARD .20

typedef enum {NONE, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, MARK_MINE, ACTIVATE, PRINT, DEBUG, QUIT} Action;

int height;
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
void printBoard(Tile* board[height][width], bool lost) {
    /*Print surrounding border*/
    printf(" ");
    for (int i = 0; i < width; i++) {
        printf("--");
    }
    printf("\n");
    
    /*Print board*/
    for (int i = 0; i < height; i++) {
        printf("| ");
        for (int j = 0; j < width; j++) {
            if (i == cursor->x && j == cursor->y) {
                if (lost) {
                    printf("☠ ");
                } else {
                    printCursor(cursor, board[i][j]->isHidden, board[i][j]->markedAsMine, board[i][j]->surroundingMines);
                }
            } else {
                if (debug) {
                    printTrueTile(board[i][j]);
                } else {
                    printTile(board[i][j], lost);
                }
            }
        }
        printf("|\n");
    }
    
    /*Print surrounding border*/
    printf(" ");
    for (int i = 0; i < width; i++) {
        printf("--");
    }
    printf("\n");
}

void markMine(Tile* board[height][width]) {
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

void setMines(Tile* board[height][width], int cursorX, int cursorY) {
    int numMines = (int) (height * width * difficulty);
    
    int* mineX = malloc(sizeof(int) * numMines);
    int* mineY = malloc(sizeof(int) * numMines);
    
    /*probably not the most optimal*/
    for (int i = 0; i < numMines; i++) {
        int x = rand() % height;
        int y = rand() % width;
        
        while (x == cursorX || inArray(mineX, x, height)) {
            x = rand() % height;
        }
        
        while (y == cursorY || inArray(mineY, y, width)) {
            y = rand() % width;
        }
        
        mineX[i] = x;
        mineY[i] = y;
    }
    
    /*place mines*/
    for (int i = 0; i < height; i++) {
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
void calculateSurroudingMines(Tile* board[height][width]) {
    for (int i = 0; i < height; i++) {
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
            } else if (i == height - 1 && j == 0) {
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
            } else if (i == height - 1 && j == width - 1) {
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
            } else if (j == 0 && i != 0 && i != height - 1) {
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
            } else if (i == height - 1 && j != 0 && j != width - 1) {
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
            } else if (j == width - 1 && i != 0 && i != height - 1) {
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

void activateTile(Tile* board[height][width], int tileX, int tileY, bool initialCall) {
    /*do nothing if current tile is marked as mine*/
    if (board[tileX][tileY]->markedAsMine) {
        return;
    }
    /*if tile has mine, game is over*/
    if (board[tileX][tileY]->hasMine && initialCall) {
        lose = true;
        printBoard(board, lose);
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
            activateTile(board, tileX + 1, tileY + 1, false);
        } else if (tileX == height - 1 && tileY == 0) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX - 1, tileY + 1, false);
        } else if (tileX == 0 && tileY == width - 1) {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX, tileY - 1, false);  
            activateTile(board, tileX + 1, tileY - 1, false);
        } else if (tileX == height - 1 && tileY == width - 1) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY - 1, false);
            activateTile(board, tileX - 1, tileY - 1, false);
        } 
        /*four edge cases*/
        else if (tileX == 0 && tileY != 0 && tileY != width - 1) {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false);
            activateTile(board, tileX + 1, tileY + 1, false);
            activateTile(board, tileX + 1, tileY - 1, false);
        } else if (tileY == 0 && tileX != 0 && tileX != height - 1) {
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);  
            activateTile(board, tileX + 1, tileY + 1, false);
            activateTile(board, tileX - 1, tileY + 1, false);
        } else if (tileX == height - 1 && tileY != 0 && tileY != width - 1) {
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false); 
            activateTile(board, tileX - 1, tileY + 1, false);
            activateTile(board, tileX - 1, tileY - 1, false);
        } else if (tileY == width - 1 && tileX != 0 && tileX != height - 1) {
            activateTile(board, tileX, tileY - 1, false);
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX + 1, tileY - 1, false);
            activateTile(board, tileX - 1, tileY - 1, false);
        }
        /*everything else*/
        else {
            activateTile(board, tileX + 1, tileY, false);
            activateTile(board, tileX - 1, tileY, false);
            activateTile(board, tileX, tileY + 1, false);
            activateTile(board, tileX, tileY - 1, false);
            activateTile(board, tileX + 1, tileY + 1, false);
            activateTile(board, tileX + 1, tileY - 1, false);
            activateTile(board, tileX - 1, tileY + 1, false);
            activateTile(board, tileX - 1, tileY - 1, false);
        }
    } else if (board[tileX][tileY]->isHidden && !board[tileX][tileY]->hasMine) {
        board[tileX][tileY]->isHidden = false;
        return;
    } else {
        return;
    }
}

void winCheck(Tile* board[height][width]) {
    bool allUncovered = true;
    for (int i = 0; i < height; i++) {
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

void revertToStartingState(Tile* board[height][width]) {
    win = false;
    lose = false;
    
    firstMove = true;
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            free(board[i][j]);
        }
    }
    free(cursor);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = createTile(i, j);
        }
    }
    cursor = createCursor(height, width);
}

/*Methods for handling user input.*/
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
    printf("q = quit\nw = move cursor up\na = move cursor left\ns = move cursor down\nd = move cursor right\nn = uncover tile\nm = mark tile as mine\n");
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
            moveCursor(cursor, 0, -1, height, width);
            break;
        case MOVE_DOWN:
            moveCursor(cursor, 1, 0, height, width);
            break;
        case MOVE_RIGHT:
            moveCursor(cursor, 0, 1, height, width);
            break;
        case MOVE_UP:
            moveCursor(cursor, -1, 0, height, width);
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

/*Methods that prompt the user for input.*/
void promptHeight() {
    char* p, s[BUFF_SIZE];
    int input;
    
    bool resolved = false;
    
    while (!resolved) {
        printf("Enter a board height (between 2 and %d):\n", MAX_RANGE);
        while (fgets(s, sizeof(s), stdin)) {
            input = strtol(s, &p, 10);
            if (p == s || *p != '\n') {
                printf("Invalid entry - must be an integer.\nEnter a board height (between 2 and %d):\n", MAX_RANGE);
            } else {
                break;  
            }
        }
        if (input >= 2 && input <= MAX_RANGE) {
            height = input;
            resolved = true;
        } else {
            printf("Invalid entry - must be between 2 and %d.\n", MAX_RANGE);
        }
    }
}

void promptWidth() {
    char* p, s[BUFF_SIZE];
    int input;
    
    bool resolved = false;
    
    while (!resolved) {
        printf("Enter a board width (between 2 and %d):\n", MAX_RANGE);
        while (fgets(s, sizeof(s), stdin)) {
            input = strtol(s, &p, 10);
            if (p == s || *p != '\n') {
                printf("Invalid entry - must be an integer.\nEnter a board width (between 2 and %d):\n", MAX_RANGE);
            } else {
                break;  
            }
        }
        if (input >= 2 && input <= MAX_RANGE) {
            width = input;
            resolved = true;
        } else {
            printf("Invalid entry - must be between 2 and %d.\n", MAX_RANGE);
        }
    }
}

void promptDifficulty() {
    char input;
    
    bool resolved = false;
    
    while (!resolved) {
        printf("Enter a difficulty:\n\te = easy\n\ti = intermediate\n\th = hard\n");
        if (scanf(" %c", &input) != -1) {
            if (input == 'e' || input == 'E') {
                difficulty = EASY;
                resolved = true;
            } else if (input == 'i' || input == 'I') {
                difficulty = INTERMEDIATE;
                resolved = true;
            } else if (input == 'h' || input == 'H') {
                difficulty = HARD;
                resolved = true;
            } else {
                printf("Invalid entry - enter e, i, or h.\n");
            }
        } else {
            printf("Invalid entry.\n");
        }
    }
}

void promptNextAction() {
    printf("Play again? (y/n)\n");
    char input;
    
    bool resolved = false;
    bool adjustDifficulty = false;
    
    while (!resolved) {
        if (scanf(" %c", &input) != -1) {
            if (input == 'n' || input == 'N') {
                play = false;
                return;
            } else if (input == 'y' || input == 'Y') {
                resolved = true;
            } else {
                printf("Enter y or n.\n");
            }
        } else {
            printf("Invalid entry.\n");
        }
    }
    
    printf("Adjust difficulty? (y/n)\n");
    
    resolved = false;
    
    while (!resolved) {
        if (scanf(" %c", &input) != -1) {
            if (input == 'n' || input == 'N') {
                resolved = true;
            } else if (input == 'y' || input == 'Y') {
                adjustDifficulty = true;
                resolved = true;
            } else {
                printf("Enter y or n.\n");
            }
        } else {
            printf("Invalid entry.\n");
        }
    }
    
    if (adjustDifficulty) {
        promptDifficulty();
    }
}

int main (int argc, char* argv[]) {
    printf("Welcome to Minesweeper. For the best experience, resize your\nterminal window to fit the entirety of the board.\n");
    /*Set difficulty, and length and width of board.*/
    if (argc != 3) {
        promptHeight();
        promptWidth();
        promptDifficulty();
    } else if (argc == 3) {
        if (*argv[1] == 's' || *argv[1] == 'S') {
            height = width = 9;
        } else if (*argv[1] == 'm' || *argv[1] == 'M') {
            height = 9;
            width = 18;
        } else if (*argv[1] == 'l' || *argv[1] == 'L') {
            height = 9;
            width = 27;
        } else {
            /*default to medium if bad entry*/
            height = 9;
            width = 18;
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
    printf("Initializing board with height = %d, width = %d, and difficulty = %s.\nPress h for a list of controls.\n", height, width, getDifficultyString());
    
    /*Initialize board with the set length and width, initially no mines.*/
    Tile* board[height][width];
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = createTile(i, j);
        }
    }
    
    /*Initialize cursor to approximate middle of board.*/
    cursor = createCursor(height, width);
    
    /*seed random number generator*/
    srand(time(NULL)); 
    
    while (play) {
        /*Print the board.*/
        printBoard(board, lose);
        
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readInput();
            evaluateInput(board);
            winCheck(board);
        } else {
            printf("Invalid entry.\n");
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
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            free(board[i][j]);
        }
    }
    free(cursor);
    
    printf("Program closed.\nThanks for playing.\n");
    return 0;
}