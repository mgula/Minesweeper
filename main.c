#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tile.h"
#include "cursor.h"

#define SMALL 7
#define MEDIUM 9
#define LARGE 11

typedef enum {NONE, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT, MARK_MINE, ACTIVATE, QUIT} Action;

int length;
int width;

bool play = true;

char currentCommand;
Action currentAction = NONE;
Cursor* cursor;

void printControls() {
    printf("w = move cursor up\n");
    printf("a = move cursor right\n");
    printf("s = move cursor down\n");
    printf("d = move cursor left\n");
    printf("q = quit\n");
    printf("space = activate at current cursor location\n");
}

void readInput() {
    switch (currentCommand) {
        case 'a':
            currentAction = MOVE_RIGHT;
            break;
        case 's':
            currentAction = MOVE_DOWN;
            break;
        case 'd':
            currentAction = MOVE_LEFT;
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
            printControls();
            break;
        case 'n':
            currentAction = ACTIVATE;
            break;
        default:
            printf("Command not recognized. Press 'h' for a list of commands.\n");
            break;
    }
}

void evaluateInput(Cursor* c) {
    switch (currentAction) {
        case NONE:
            break;
        case MOVE_UP:
            moveCursor(c, 0, -1, width, length);
            break;
        case MOVE_DOWN:
            moveCursor(c, 0, 1, width, length);
            break;
        case MOVE_LEFT:
            moveCursor(c, 1, 0, width, length);
            break;
        case MOVE_RIGHT:
            moveCursor(c, -1, 0, width, length);
            break;
        case QUIT:
            play = false;
            break;
        case MARK_MINE:
            break;
        default:
            break;
    }
    currentAction = NONE;
}

void printBoard(Tile* board[][width]) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (i == cursor->y && j == cursor->x) {
                printf("⇲ ");
            } else {
                printTile(board[i][j]);
            }
        }
        printf("\n");
    }
}

int main (int argc, char* argv[]) {
    /*Set length and width of board.*/
    if (argc == 1) {
        /*For now, default to medium.*/
        length = width = MEDIUM;
    } else if (argc == 2) {
        if (*argv[1] == 's' || *argv[1] == 'S') {
            printf("Initializing board with size = small (4x4).\n");
            length = width = SMALL;
        } else if (*argv[1] == 'm' || *argv[1] == 'M') {
            printf("Initializing board with size = medium (6x6).\n");
            length = width = MEDIUM;
        } else if (*argv[1] == 'l' || *argv[1] == 'L') {
            printf("Initializing board with size = large (8x8).\n");
            length = width = LARGE;
        }
    } 
    
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
            evaluateInput(cursor);
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