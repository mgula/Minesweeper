/*Author: Marcus Gula
Command line version of the game Minesweeper. Some notes: Board doesn't 
have it's own header + c file because it's difficult to pass the length and 
width of the board to methods in other files, especially since both of those
dimensions are variable (the player decides on the command line or at runtime).

To me, it seemed the simplest solution was to make length and width global in 
this file so all methods have easy access to them.

This program does not support changing the size of the board in between games. All
games of a session will be of the same board size.

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

#define MAX_RANGE 50
#define BUFF_SIZE 50

#define EASY .05
#define INTERMEDIATE .10
#define HARD .20

/*TODO
-remove debug
-print info: number of mines, number of mines marked
-add control to activate all surrounding tiles of a 
tile where surroundingMines == surroundingMarkedAsMine*/

int height;
int width;

double difficulty;

bool play = true;
bool firstMove = true;

bool win = false;
bool lose = false;

char currentCommand = 'h';

Cursor* cursor;

bool debug = false;

/*Methods that return/print some helpful strings.*/
char* getDifficultyString();
void printControls();

/*Methods for operations on the board.*/
void printBoard(Tile* board[height][width], bool lost);
void markAsMine(Tile* board[height][width]);
bool inArray(int* arr, int x, int l);
void setMines(Tile* board[height][width], int cursorX, int cursorY);
void calculateSurroudingMines(Tile* board[height][width]);
void activateTile(Tile* board[height][width], int tileX, int tileY, bool initialCall);
void winCheck(Tile* board[height][width]);
void revertToStartingState(Tile* board[height][width]);

/*Reads and executes user input during a game.*/
void readAndExecuteInput(Tile* board[height][width]);

/*Methods that directly prompt the user for input.*/
void promptHeight();
void promptWidth();
void promptDifficulty();
void promptNextAction();

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
            /*Default to medium if bad entry*/
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
            /*Default to intermediate if bad entry*/
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
    
    /*Seed random number generator*/
    srand(time(NULL)); 
    
    while (play) {
        /*Print the board.*/
        printBoard(board, lose);
        
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readAndExecuteInput(board);
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

/*Method used by main() to print the current difficulty.*/
char* getDifficultyString() {
    if (difficulty == EASY) {
        return "easy";
    } else if (difficulty == INTERMEDIATE) {
        return "intermediate";
    } else if (difficulty == HARD) {
        return "hard";
    }
}

/*Prints a list of all keys used.*/
void printControls() {
    printf("q = quit\nw = move cursor up\na = move cursor left\ns = move cursor down\nd = move cursor right\nn = uncover tile\nm = mark tile as mine\n");
}

/*Print the entire board and cursor.*/
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

/*Mark the current tile as a mine. This can be used erroneously by the player,
it's up to their deduction skills.*/
void markAsMine(Tile* board[height][width]) {
    board[cursor->x][cursor->y]->markedAsMine = !board[cursor->x][cursor->y]->markedAsMine;
}

/*Used by setMine to check if x exists in arr more than once.*/
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

/*Places mines (only after the first move - it won't generate mines at
the cursor's x or y location). Sets mines simply by setting certain tile's
hasMine boolean to true.*/
void setMines(Tile* board[height][width], int cursorX, int cursorY) {
    int numMines = (int) (height * width * difficulty);
    
    int* mineX = malloc(sizeof(int) * numMines);
    int* mineY = malloc(sizeof(int) * numMines);
    
    /*Probably not the most optimal solution.*/
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
    
    /*Place mines.*/
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < numMines; k++) {
                if (board[i][j]->x == mineX[k] && board[i][j]->y == mineY[k]) {
                    board[i][j]->hasMine = true;
                    board[i][j]->surroundingMines = -1; //set to -1 so activateTile doesn't get confused
                }
            }
        }
    }
    
    free(mineX);
    free(mineY);
}

/*Calculates the number of surrounding mines for every tile on the board. Uses an
admittedly inefficient "brute force" approach.*/
void calculateSurroudingMines(Tile* board[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j]->hasMine) {
                //These cases have already been set to -1 by setMines().
                continue;
            }
            /*Four corner cases - only 3 surrounding tiles*/
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
            /*Edge cases - only 5 surrounding tiles*/
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
            /*Everything else - 8 surrounding tiles*/
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

/*Actives the tile under the cursor. Calls itself to activate all tiles around
a tile with 0 mines around it.*/
void activateTile(Tile* board[height][width], int tileX, int tileY, bool initialCall) {
    /*Do nothing if current tile is marked as mine*/
    if (board[tileX][tileY]->markedAsMine) {
        return;
    }
    /*If tile has mine, game is over*/
    if (board[tileX][tileY]->hasMine && initialCall) {
        lose = true;
        printBoard(board, lose);
        return;
    }
    /*Recursively check around tiles with 0 surrounding mines*/
    if (board[tileX][tileY]->isHidden && board[tileX][tileY]->surroundingMines == 0 || firstMove) {
        if (firstMove) {
            firstMove = false;
        }
        board[tileX][tileY]->isHidden = false;
        /*Four corner cases - only 3 surrounding tiles*/
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
        /*Edge cases - only 5 surrounding tiles*/
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
        /*Everything else - 8 surrounding tiles*/
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
        /*If not a 0 tile or mine tile, just uncover it.*/
        board[tileX][tileY]->isHidden = false;
        return;
    } else {
        return;
    }
}

/*Check to see if the player won. A game is won if all non-mine tiles are uncovered*/
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

/*Make a new (same-sized) board, set some booleans back to starting state. */
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

/*Read the currentCommand char and set the corresponding action.*/
void readAndExecuteInput(Tile* board[height][width]) {
    switch (currentCommand) {
        case 'a':
            //move left
            moveCursor(cursor, 0, -1, height, width);
            break;
        case 's':
            //move down
            moveCursor(cursor, 1, 0, height, width);
            break;
        case 'd':
            //move right
            moveCursor(cursor, 0, 1, height, width);
            break;
        case 'w':
            //move up
            moveCursor(cursor, -1, 0, height, width);
            break;
        case 'q':
            play = false;
            break;
        case 'm':
            markAsMine(board);
            break;
        case 'h':
            printControls();
            break;
        case 'n':
            if (firstMove) {
                setMines(board, cursor->x, cursor->y);
                calculateSurroudingMines(board);
            }
            activateTile(board, cursor->x, cursor->y, true);
            break;
        case 'j':
            debug = !debug;
            break;
        default:
            printf("Command not recognized. Press 'h' for a list of commands.\n");
            break;
    }
}

/*Prompts the user to enter a board height. Ensures the user enters an integer.*/
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

/*Prompts the user to enter a board width. Ensures the user enters an integer.*/
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

/*Prompts the user to select a difficulty (easy, intermediate, or hard).*/
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

/*Prompts to user to select between playing another game or quitting.*/
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