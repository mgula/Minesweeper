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
    c - toggle unicode/ascii
    b - uncover all surrounding tiles
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
#define BASE 10

#define EASY .05
#define INTERMEDIATE .10
#define HARD .20

int height;
int width;

double difficulty;

int numMines = 0;
int minesMarked = 0;

bool play = true;
bool firstMove = true;

bool win = false;
bool lose = false;

char currentCommand = 'h';

bool charStyle = CHARSTYLE;

Cursor* cursor;

/*Methods that return/print some helpful strings.*/
char* getDifficultyString();
void printControls();
void printMineInfo();

/*Methods for operations on the board.*/
void printBoard(Tile* board[height][width]);
void markAsMine(Tile* board[height][width]);
bool inArray(int* arr, int x, int l);
void setMines(Tile* board[height][width], int cursorX, int cursorY);
void calculateSurroudingMines(Tile* board[height][width]);
void incrementTilesAroundMine(Tile* board[height][width], int tileX, int tileY);
void activateTile(Tile* board[height][width], int tileX, int tileY, bool initialCall);
void activateSurroundingTiles(Tile* board[height][width], int tileX, int tileY);
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
        /*Print mine info.*/
        if (!firstMove) {
            printMineInfo();
        }
        
        /*Print the board.*/
        printBoard(board);
        
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readAndExecuteInput(board);
            winCheck(board);
        } else {
            printf("Invalid entry.\n");
        }
        
        /*Evaluate winning and losing conditions.*/
        if (win || lose) {
            printBoard(board);
            if (win) {
                printf("Congratulations, you won!\n");
            } else if (lose) {
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
    printf("q = quit\nw = move cursor up\na = move cursor left\ns = move cursor down\nd = move cursor right\nc = toggle unicode/ascii\nb = uncover surrounding tiles - base tile must be uncoverred, with surrouding mines marked\nn = uncover tile\nm = mark tile as mine\n");
}

void printMineInfo() {
    printf("Total mines: %d \t Mines marked: %d\n", numMines, minesMarked);
}

/*Print the entire board and cursor.*/
void printBoard(Tile* board[height][width]) {
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
                printCursor(cursor, board[i][j]->isHidden, board[i][j]->markedAsMine, board[i][j]->surroundingMines, lose, win, charStyle);
            } else {
                printTile(board[i][j], lose, win, charStyle);
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
    if (board[cursor->x][cursor->y]->markedAsMine) {
        minesMarked--;
    } else {
        minesMarked++;
    }
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
the cursor's x and y location). Sets mines simply by setting certain tile's
hasMine boolean to true.*/
void setMines(Tile* board[height][width], int cursorX, int cursorY) {
    int ideal = (int) (height * width * difficulty);
    
    int* mineX = malloc(sizeof(int) * ideal);
    int* mineY = malloc(sizeof(int) * ideal);
    
    /*Probably not the most optimal solution.*/
    for (int i = 0; i < ideal; i++) {
        int x = rand() % height;
        int y = rand() % width;
        
        while ((x == cursorX && y == cursorY) || inArray(mineX, x, height) || inArray(mineY, y, width)) {
            x = rand() % height;
            y = rand() % width;
        }
        
        mineX[i] = x;
        mineY[i] = y;
    }
    
    /*Place mines.*/
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            bool unique = true;
            for (int k = 0; k < ideal; k++) {
                if (board[i][j]->x == mineX[k] && board[i][j]->y == mineY[k]) {
                    if (unique) {
                        numMines++;
                        unique = false;
                    }
                    board[i][j]->hasMine = true;
                    board[i][j]->surroundingMines = -1; //set to -1 so activateTile doesn't get confused
                    
                }
            }
        }
    }
    
    free(mineX);
    free(mineY);
}

/*Calculates the number of surrounding mines for every tile on the board.*/
void calculateSurroudingMines(Tile* board[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j]->hasMine) {
                incrementTilesAroundMine(board, i, j);
            }
        }
    }
}

/*Add one to surroundingMines for all (non-mine) tiles surrounding a mine.*/
void incrementTilesAroundMine(Tile* board[height][width], int tileX, int tileY) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int this_x = tileX + i;
            int this_y = tileY + j;
            
            if (i == 0 && j == 0) {
                continue;
            } else if (this_x < 0 || this_x >= height) {
                continue;
            } else if (this_y < 0 || this_y >= width) {
                continue;
            } else if (board[this_x][this_y]->hasMine == true) {
                continue;
            } else {
                board[this_x][this_y]->surroundingMines += 1;
            }
        }
    }
}

/*Actives the tile under the cursor. Calls itself to activate all tiles around
a tile with surroundingMines == 0.*/
void activateTile(Tile* board[height][width], int tileX, int tileY, bool initialCall) {
    /*Do nothing if current tile is marked as mine or already activated*/
    if (board[tileX][tileY]->markedAsMine || !board[tileX][tileY]->isHidden) {
        return;
    }
    
    /*If tile has mine, game is over*/
    if (board[tileX][tileY]->hasMine && initialCall) {
        lose = true;
        return;
    }
    
    if (board[tileX][tileY]->isHidden && !board[tileX][tileY]->hasMine) {
        board[tileX][tileY]->isHidden = false;
        if (board[tileX][tileY]->surroundingMines == 0 || firstMove) {
            firstMove = false;
            for (int i = -1; i <= 1; i++){
                for (int j = -1; j <= 1; j++){
                    int this_x = tileX + i;
                    int this_y = tileY + j;
                    
                    if (i == 0 && j == 0) {
                        continue;
                    } else if(this_x < 0 || this_x >= height) {
                        continue;
                    } else if(this_y < 0 || this_y >= width) {
                        continue;
                    }
                    activateTile(board, tileX+i,tileY+j, false);
                }
            }
        }
    }
}

/*Uncover all tiles surrounding an uncovered tile with all mines marked.*/
void activateSurroundingTiles(Tile* board[height][width], int cursorX, int cursorY) {
    if (firstMove || board[cursorX][cursorY]->isHidden || board[cursorX][cursorY]->surroundingMines == 0) {
        return;
    }
    
    int surroundingMarkedAsMine = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int this_x = cursorX + i;
            int this_y = cursorY + j;
            
            if (i == 0 && j == 0) {
                continue;
            } else if (this_x < 0 || this_x >= height) {
                continue;
            } else if (this_y < 0 || this_y >= width) {
                continue;
            } else if (board[this_x][this_y]->markedAsMine) {
                surroundingMarkedAsMine += 1;
            }
        }
    }
    
    if (board[cursorX][cursorY]->surroundingMines != surroundingMarkedAsMine) {
        return;
    }
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int this_x = cursorX + i;
            int this_y = cursorY + j;
            
            if (i == 0 && j == 0) {
                continue;
            } else if (this_x < 0 || this_x >= height) {
                continue;
            } else if (this_y < 0 || this_y >= width) {
                continue;
            } else if (board[this_x][this_y]->markedAsMine) {
                continue;
            } else if (board[this_x][this_y]->hasMine) {
                lose = true;
                return;
            } else {
                activateTile(board, this_x, this_y, false);
            }
        }
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
    
    minesMarked = 0;
    numMines = 0;
    
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
            /*move left*/
            moveCursor(cursor, 0, -1, height, width);
            break;
        case 's':
            /*move down*/
            moveCursor(cursor, 1, 0, height, width);
            break;
        case 'd':
            /*move right*/
            moveCursor(cursor, 0, 1, height, width);
            break;
        case 'w':
            /*move up*/
            moveCursor(cursor, -1, 0, height, width);
            break;
        case 'q':
            play = false;
            break;
        case 'c':
            charStyle = !charStyle;
            break;
        case 'b':
            activateSurroundingTiles(board, cursor->x, cursor->y);
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
            input = strtol(s, &p, BASE);
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
            input = strtol(s, &p, BASE);
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
    char input[BUFF_SIZE];
    
    bool resolved = false;
    
    while (!resolved) {
        printf("Enter a difficulty:\n\te = easy\n\ti = intermediate\n\th = hard\n");
        while (fgets(input, sizeof(input), stdin)) {
            if (input[0] == 'e' || input[0] == 'E') {
                difficulty = EASY;
                resolved = true;
                break;
            } else if (input[0] == 'i' || input[0] == 'I') {
                difficulty = INTERMEDIATE;
                resolved = true;
                break;
            } else if (input[0] == 'h' || input[0] == 'H') {
                difficulty = HARD;
                resolved = true;
                break;
            } else {
                printf("Invalid entry - enter e, i, or h.\n");
            }
        }
    }
}

/*Prompts to user to select between playing another game or quitting.*/
void promptNextAction() {
    printf("Play again? (y/n)\n");
    char input[BUFF_SIZE];
    
    bool resolved = false;
    bool adjustDifficulty = false;
    
    while (!resolved) {
        while (fgets(input, sizeof(input), stdin)) {
            if (input[0] == 'n' || input[0] == 'N') {
                play = false;
                return;
            } else if (input[0] == 'y' || input[0] == 'Y') {
                resolved = true;
                break;
            } else {
                printf("Enter y or n.\n");
            }
        }
    }
    
    printf("Adjust difficulty? (y/n)\n");
    
    resolved = false;
    
    while (!resolved) {
        while (fgets(input, sizeof(input), stdin)) {
            if (input[0] == 'n' || input[0] == 'N') {
                resolved = true;
                break;
            } else if (input[0] == 'y' || input[0] == 'Y') {
                adjustDifficulty = true;
                resolved = true;
                break;
            } else {
                printf("Enter y or n.\n");
            }
        }
    }
    
    if (adjustDifficulty) {
        promptDifficulty();
    }
}