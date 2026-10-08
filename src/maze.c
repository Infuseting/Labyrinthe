#include "maze.h"
#include <stdlib.h>
#include <stddef.h>

bool areValidDimensions(int height, int width) {
    return (height >= MIN_HEIGHT && width >= MIN_WIDTH && height % 2 == 1 && width % 2 == 1);
}

Maze* createMaze(int height, int width) {
    if (!areValidDimensions(height, width)) {
        return NULL;
    }

    Maze* maze = (Maze*)malloc(sizeof(Maze));
    if (!maze) {
        return NULL;
    }

    maze->height = height;
    maze->width = width;
    maze->playerPosition.x = 1; // Starting position
    maze->playerPosition.y = 1; // Starting position

    if (!initMaze(maze, height, width)) {
        free(maze);
        return NULL;
    }

    return maze;
}

void freeMaze(Maze* maze) {
    if (maze) {
        if (maze->tiles) {
            for (int i = 0; i < maze->height; ++i) {
                free(maze->tiles[i]);
            }
            free(maze->tiles);
        }
        free(maze);
    }
}

bool initMaze(Maze* maze, int height, int width) {
    maze->tiles = (TileType**)malloc(height * sizeof(TileType*));
    if (!maze->tiles) {
        return false;
    }

    for (int i = 0; i < height; ++i) {
        maze->tiles[i] = (TileType*)malloc(width * sizeof(TileType));
        if (!maze->tiles[i]) {
            for (int j = 0; j < i; ++j) {
                free(maze->tiles[j]);
            }
            free(maze->tiles);
            return false;
        }
    }

    return true;
}

void freeMazeGrid(Maze* maze) {
    if (maze && maze->tiles) {
        for (int i = 0; i < maze->height; ++i) {
            free(maze->tiles[i]);
        }
        free(maze->tiles);
        maze->tiles = NULL;
    }
}

bool isValidMove(const Maze* maze, int x, int y) {
    if (!maze) return false;
    if (x < 0 || x >= maze->width || y < 0 || y >= maze->height) {
        return false;
    }
    return maze->tiles[y][x] != WALL;
}

bool movePlayer(Maze* maze, int dx, int dy) {
    if (!maze) return false;

    int newX = maze->playerPosition.x + dx;
    int newY = maze->playerPosition.y + dy;

    if (isValidMove(maze, newX, newY)) {
        maze->playerPosition.x = newX;
        maze->playerPosition.y = newY;
        return true;
    }
    return false;
}

bool movePlayerDirection(Maze* maze, char direction) {
    if (!maze) return false;

    switch (direction) {
        case MOVE_UP:
        case 'Z':
            return movePlayer(maze, 0, -1);
        case MOVE_DOWN:
        case 'S':
            return movePlayer(maze, 0, 1);
        case MOVE_LEFT:
        case 'Q':
            return movePlayer(maze, -1, 0);
        case MOVE_RIGHT:
        case 'D':
            return movePlayer(maze, 1, 0);
        default:
            return false;
    }
}

char getChar(const Maze* maze, Coords coords) {
    if (!maze || coords.x < 0 || coords.x >= maze->width || coords.y < 0 || coords.y >= maze->height) {
        return '?';
    }

    if (maze->playerPosition.x == coords.x && maze->playerPosition.y == coords.y) {
        return SYMBOL_PLAYER; 
    }

    TileType tile = maze->tiles[coords.y][coords.x];
    if (tile == PATH) return SYMBOL_PATH;
    if (tile == EXIT) return SYMBOL_EXIT;
    if (tile == WALL) return SYMBOL_WALL;

    return '?';
}

bool isMazeSolved(const Maze* maze) {
    if (!maze) return false;
    return (maze->playerPosition.x == maze->end.x && maze->playerPosition.y == maze->end.y);
}

