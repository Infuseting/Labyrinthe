#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

typedef struct {
    int r1;
    int c1;
    int r2;
    int c2;
    int wall_row;
    int wall_col;
} Wall;

Maze *maze_create(int height, int width) {
    if (height < 3 || width < 3 || height % 2 == 0 || width % 2 == 0) {
        return NULL;
    }

    Maze *maze = malloc(sizeof(Maze));
    if (!maze) {
        return NULL;
    }

    maze->height = height;
    maze->width = width;
    maze->player.x = 1;
    maze->player.y = 0;
    maze->exit.x = width - 2;
    maze->exit.y = height - 1;

    maze->grid = malloc((size_t)height * sizeof(Cell *));
    if (!maze->grid) {
        free(maze);
        return NULL;
    }

    for (int r = 0; r < height; r++) {
        maze->grid[r] = malloc((size_t)width * sizeof(Cell));
        if (!maze->grid[r]) {
            for (int prev = 0; prev < r; prev++) {
                free(maze->grid[prev]);
            }
            free(maze->grid);
            free(maze);
            return NULL;
        }
        for (int c = 0; c < width; c++) {
            maze->grid[r][c] = WALL;
        }
    }

    return maze;
}

void maze_free(Maze *maze) {
    if (!maze) {
        return;
    }

    if (maze->grid) {
        for (int r = 0; r < maze->height; r++) {
            free(maze->grid[r]);
        }
        free(maze->grid);
    }

    free(maze);
}

int maze_generate(Maze *maze) {
    if (!maze || !maze->grid || maze->height < 3 || maze->width < 3 ||
        maze->height % 2 == 0 || maze->width % 2 == 0) {
        return -1;
    }

    for (int r = 0; r < maze->height; r++) {
        for (int c = 0; c < maze->width; c++) {
            maze->grid[r][c] = WALL;
        }
    }

    int num_cell_rows = (maze->height - 1) / 2;
    int num_cell_cols = (maze->width - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            maze->grid[2 * r + 1][2 * c + 1] = EMPTY;
        }
    }

    int *cell_ids = malloc((size_t)total_cells * sizeof(int));
    if (!cell_ids) {
        return -1;
    }

    for (int i = 0; i < total_cells; i++) {
        cell_ids[i] = i;
    }

    int num_h_walls = num_cell_rows * (num_cell_cols - 1);
    int num_v_walls = (num_cell_rows - 1) * num_cell_cols;
    int total_walls = num_h_walls + num_v_walls;

    Wall *walls = malloc((size_t)total_walls * sizeof(Wall));
    if (!walls) {
        free(cell_ids);
        return -1;
    }

    int wall_count = 0;

    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols - 1; c++) {
            walls[wall_count].r1 = r;
            walls[wall_count].c1 = c;
            walls[wall_count].r2 = r;
            walls[wall_count].c2 = c + 1;
            walls[wall_count].wall_row = 2 * r + 1;
            walls[wall_count].wall_col = 2 * c + 2;
            wall_count++;
        }
    }

    for (int r = 0; r < num_cell_rows - 1; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            walls[wall_count].r1 = r;
            walls[wall_count].c1 = c;
            walls[wall_count].r2 = r + 1;
            walls[wall_count].c2 = c;
            walls[wall_count].wall_row = 2 * r + 2;
            walls[wall_count].wall_col = 2 * c + 1;
            wall_count++;
        }
    }

    int remaining_walls = wall_count;
    int opened_walls = 0;
    int target_opened = total_cells - 1;

    while (opened_walls < target_opened && remaining_walls > 0) {
        int rand_index = rand() % remaining_walls;
        Wall selected_wall = walls[rand_index];

        walls[rand_index] = walls[remaining_walls - 1];
        remaining_walls--;

        int idx1 = selected_wall.r1 * num_cell_cols + selected_wall.c1;
        int idx2 = selected_wall.r2 * num_cell_cols + selected_wall.c2;

        int id1 = cell_ids[idx1];
        int id2 = cell_ids[idx2];

        if (id1 != id2) {
            maze->grid[selected_wall.wall_row][selected_wall.wall_col] = EMPTY;
            opened_walls++;

            for (int i = 0; i < total_cells; i++) {
                if (cell_ids[i] == id2) {
                    cell_ids[i] = id1;
                }
            }
        }
    }

    free(walls);
    free(cell_ids);

    maze->player.x = 1;
    maze->player.y = 0;
    maze->exit.x = maze->width - 2;
    maze->exit.y = maze->height - 1;

    maze->grid[maze->player.y][maze->player.x] = EMPTY;
    maze->grid[maze->exit.y][maze->exit.x] = EMPTY;

    return 0;
}

void maze_print(const Maze *maze) {
    if (!maze || !maze->grid) {
        return;
    }

    for (int r = 0; r < maze->height; r++) {
        for (int c = 0; c < maze->width; c++) {
            if (r == maze->player.y && c == maze->player.x) {
                putchar('o');
            } else if (r == maze->exit.y && c == maze->exit.x) {
                putchar('-');
            } else {
                putchar((char)maze->grid[r][c]);
            }
        }
        putchar('\n');
    }
}
