#include <stdio.h>
#include <stdlib.h>
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

char currentCommand = 'h';
Action currentAction = NONE;

Cursor* cursor;

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
            break;
        default:
            break;
    }
    currentAction = NONE;
}

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
            board[i][j] = createTile(j, i);
        }
    }
    
    /*Initialize cursor to approximate middle of board.*/
    cursor = createCursor(width, length);
    
    while (play) {
        /*Print board.*/
        printBoard(board);
        
        /*Prompt command.*/
        if (scanf(" %c", &currentCommand) != -1) {
            readInput();
            evaluateInput(board);
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