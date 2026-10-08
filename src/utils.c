#include "utils.h"
#include "maze.h"
#include <stdio.h>
#include <stdlib.h>




bool getChoice(int *choice, int min, int max) {
    int input;
    if (scanf("%d", &input) != 1) {
        clearInputBuffer();
        return false;
    }
    clearInputBuffer();
    if (input < min || input > max) {
        return false;
    }
    *choice = input;
    return true;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void clearScreen() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif
}

void displayMaze(const Maze* maze) {
    if (!maze || !maze->tiles) {
        printf("Maze is not initialized.\n");
        return;
    }

    for (int y = 0; y < maze->height; ++y) {
        for (int x = 0; x < maze->width; ++x) {
            Coords coords = {x, y};
            char tileChar = getChar(maze, coords);
            putchar(tileChar);
        }
        putchar('\n');
    }
}

