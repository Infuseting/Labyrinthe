#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"

int main(void) {
    srand((unsigned int)time(NULL));

    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    if (!maze) {
        return EXIT_FAILURE;
    }

    if (maze_generate(maze) != 0) {
        maze_free(maze);
        return EXIT_FAILURE;
    }

    maze_print(maze);
    maze_free(maze);

    return EXIT_SUCCESS;
}
