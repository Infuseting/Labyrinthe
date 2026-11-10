#ifndef MAZE_H
#define MAZE_H

#define DEFAULT_HEIGHT 11
#define DEFAULT_WIDTH 25

typedef enum {
    WALL = '#',
    EMPTY = ' ',
} Cell;

typedef struct {
    int x;
    int y;
} Coords;

typedef struct {
    int height;
    int width;
    Coords player;
    Coords exit;
    Cell **grid;
} Maze;

Maze *maze_create(int height, int width);
void maze_free(Maze *maze);
int maze_generate(Maze *maze);
void maze_print(const Maze *maze);

#endif
