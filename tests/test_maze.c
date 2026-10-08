#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <minunit.h>
#include "maze.h"

/* --- Tests pour areValidDimensions --- */

MU_TEST(test_areValidDimensions_valid) {
    mu_check(areValidDimensions(11, 25));
    mu_check(areValidDimensions(3, 3));
    mu_check(areValidDimensions(5, 7));
    mu_check(areValidDimensions(21, 51));
}

MU_TEST(test_areValidDimensions_even_numbers) {
    mu_check(!areValidDimensions(10, 25));
    mu_check(!areValidDimensions(11, 24));
    mu_check(!areValidDimensions(10, 24));
    mu_check(!areValidDimensions(4, 4));
}

MU_TEST(test_areValidDimensions_below_minimum) {
    mu_check(!areValidDimensions(1, 25));
    mu_check(!areValidDimensions(11, 1));
    mu_check(!areValidDimensions(1, 1));
    mu_check(!areValidDimensions(0, 0));
    mu_check(!areValidDimensions(-5, 25));
    mu_check(!areValidDimensions(11, -7));
}

/* --- Tests pour createMaze et freeMaze --- */

MU_TEST(test_createMaze_success) {
    Maze* m = createMaze(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(m != NULL);
    mu_assert_int_eq(DEFAULT_HEIGHT, m->height);
    mu_assert_int_eq(DEFAULT_WIDTH, m->width);
    mu_assert_int_eq(1, m->playerPosition.x);
    mu_assert_int_eq(1, m->playerPosition.y);
    mu_check(m->tiles != NULL);

    for (int i = 0; i < m->height; ++i) {
        mu_check(m->tiles[i] != NULL);
    }
    freeMaze(m);
}

MU_TEST(test_createMaze_invalid_dimensions) {
    mu_check(createMaze(10, 25) == NULL);
    mu_check(createMaze(11, 20) == NULL);
    mu_check(createMaze(1, 1) == NULL);
    mu_check(createMaze(-3, -3) == NULL);
}

MU_TEST(test_freeMaze_null_pointer) {
    /* Verifie que freeMaze gere gracieusement le pointeur NULL sans plantage */
    freeMaze(NULL);
    mu_check(true);
}

/* --- Tests pour initMaze et freeMazeGrid --- */

MU_TEST(test_initMaze_and_freeMazeGrid) {
    Maze m;
    m.height = 7;
    m.width = 9;
    bool ok = initMaze(&m, 7, 9);
    mu_check(ok);
    mu_check(m.tiles != NULL);
    for (int i = 0; i < 7; ++i) {
        mu_check(m.tiles[i] != NULL);
    }

    freeMazeGrid(&m);
    mu_check(m.tiles == NULL);
}

MU_TEST(test_freeMazeGrid_null_pointer) {
    freeMazeGrid(NULL);
    Maze m;
    m.tiles = NULL;
    freeMazeGrid(&m);
    mu_check(true);
}

/* --- Tests pour isValidMove --- */

MU_TEST(test_isValidMove_null_maze) {
    mu_check(!isValidMove(NULL, 1, 1));
}

MU_TEST(test_isValidMove_out_of_bounds) {
    Maze* m = createMaze(5, 5);
    mu_check(!isValidMove(m, -1, 1));
    mu_check(!isValidMove(m, 1, -1));
    mu_check(!isValidMove(m, 5, 1));
    mu_check(!isValidMove(m, 1, 5));
    mu_check(!isValidMove(m, 10, 10));
    freeMaze(m);
}

MU_TEST(test_isValidMove_tile_types) {
    Maze* m = createMaze(5, 5);
    m->tiles[1][1] = PATH;
    m->tiles[1][2] = WALL;
    m->tiles[1][3] = EXIT;

    mu_check(isValidMove(m, 1, 1));  /* PATH est accessible */
    mu_check(!isValidMove(m, 2, 1)); /* WALL est bloque */
    mu_check(isValidMove(m, 3, 1));  /* EXIT est accessible */
    freeMaze(m);
}

/* --- Tests pour movePlayer --- */

MU_TEST(test_movePlayer_null_maze) {
    mu_check(!movePlayer(NULL, 1, 0));
}

MU_TEST(test_movePlayer_valid_movement) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 1;
    m->playerPosition.y = 1;
    m->tiles[1][1] = PATH;
    m->tiles[1][2] = PATH; /* Vers la droite */

    bool moved = movePlayer(m, 1, 0);
    mu_check(moved);
    mu_assert_int_eq(2, m->playerPosition.x);
    mu_assert_int_eq(1, m->playerPosition.y);
    freeMaze(m);
}

MU_TEST(test_movePlayer_blocked_by_wall) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 1;
    m->playerPosition.y = 1;
    m->tiles[1][1] = PATH;
    m->tiles[0][1] = WALL; /* Vers le haut */

    bool moved = movePlayer(m, 0, -1);
    mu_check(!moved);
    mu_assert_int_eq(1, m->playerPosition.x);
    mu_assert_int_eq(1, m->playerPosition.y);
    freeMaze(m);
}

MU_TEST(test_movePlayer_blocked_by_bounds) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 0;
    m->playerPosition.y = 0;

    bool moved = movePlayer(m, -1, 0);
    mu_check(!moved);
    mu_assert_int_eq(0, m->playerPosition.x);
    mu_assert_int_eq(0, m->playerPosition.y);
    freeMaze(m);
}

/* --- Tests pour movePlayerDirection --- */

MU_TEST(test_movePlayerDirection_null_maze) {
    mu_check(!movePlayerDirection(NULL, 'z'));
}

MU_TEST(test_movePlayerDirection_cardinal_directions) {
    Maze* m = createMaze(5, 5);
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m->tiles[y][x] = WALL;
        }
    }
    m->tiles[2][2] = PATH;
    m->tiles[1][2] = PATH; /* Haut */
    m->tiles[3][2] = PATH; /* Bas */
    m->tiles[2][1] = PATH; /* Gauche */
    m->tiles[2][3] = PATH; /* Droite */

    /* Deplacement Haut ('z') */
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;
    mu_check(movePlayerDirection(m, MOVE_UP));
    mu_assert_int_eq(2, m->playerPosition.x);
    mu_assert_int_eq(1, m->playerPosition.y);

    /* Deplacement Bas ('s') */
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;
    mu_check(movePlayerDirection(m, MOVE_DOWN));
    mu_assert_int_eq(2, m->playerPosition.x);
    mu_assert_int_eq(3, m->playerPosition.y);

    /* Deplacement Gauche ('q') */
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;
    mu_check(movePlayerDirection(m, MOVE_LEFT));
    mu_assert_int_eq(1, m->playerPosition.x);
    mu_assert_int_eq(2, m->playerPosition.y);

    /* Deplacement Droite ('d') */
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;
    mu_check(movePlayerDirection(m, MOVE_RIGHT));
    mu_assert_int_eq(3, m->playerPosition.x);
    mu_assert_int_eq(2, m->playerPosition.y);

    freeMaze(m);
}

MU_TEST(test_movePlayerDirection_uppercase_support) {
    Maze* m = createMaze(5, 5);
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m->tiles[y][x] = WALL;
        }
    }
    m->tiles[2][2] = PATH;
    m->tiles[1][2] = PATH; /* Haut */
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;

    mu_check(movePlayerDirection(m, 'Z'));
    mu_assert_int_eq(2, m->playerPosition.x);
    mu_assert_int_eq(1, m->playerPosition.y);
    freeMaze(m);
}

MU_TEST(test_movePlayerDirection_invalid_keys) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 2;
    m->playerPosition.y = 2;

    mu_check(!movePlayerDirection(m, 'x'));
    mu_check(!movePlayerDirection(m, ' '));
    mu_check(!movePlayerDirection(m, '1'));
    mu_assert_int_eq(2, m->playerPosition.x);
    mu_assert_int_eq(2, m->playerPosition.y);
    freeMaze(m);
}

/* --- Tests pour getChar --- */

MU_TEST(test_getChar_null_and_out_of_bounds) {
    Coords c = {1, 1};
    mu_assert_int_eq('?', getChar(NULL, c));

    Maze* m = createMaze(5, 5);
    Coords negX = {-1, 1};
    Coords negY = {1, -1};
    Coords overX = {5, 1};
    Coords overY = {1, 5};
    mu_assert_int_eq('?', getChar(m, negX));
    mu_assert_int_eq('?', getChar(m, negY));
    mu_assert_int_eq('?', getChar(m, overX));
    mu_assert_int_eq('?', getChar(m, overY));
    freeMaze(m);
}

MU_TEST(test_getChar_player_position) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 2;
    m->playerPosition.y = 3;
    Coords playerCoords = {2, 3};

    mu_assert_int_eq(SYMBOL_PLAYER, getChar(m, playerCoords));
    freeMaze(m);
}

MU_TEST(test_getChar_tile_symbols) {
    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 0;
    m->playerPosition.y = 0; /* Eloigne le joueur des cases testees */

    m->tiles[1][1] = PATH;
    m->tiles[1][2] = WALL;
    m->tiles[1][3] = EXIT;

    Coords cPath = {1, 1};
    Coords cWall = {2, 1};
    Coords cExit = {3, 1};

    mu_assert_int_eq(SYMBOL_PATH, getChar(m, cPath));
    mu_assert_int_eq(SYMBOL_WALL, getChar(m, cWall));
    mu_assert_int_eq(SYMBOL_EXIT, getChar(m, cExit));
    freeMaze(m);
}

/* --- Tests pour isMazeSolved --- */

MU_TEST(test_isMazeSolved_logic) {
    mu_check(!isMazeSolved(NULL));

    Maze* m = createMaze(5, 5);
    m->playerPosition.x = 1;
    m->playerPosition.y = 1;
    m->end.x = 3;
    m->end.y = 3;

    mu_check(!isMazeSolved(m));

    m->playerPosition.x = 3;
    m->playerPosition.y = 3;
    mu_check(isMazeSolved(m));

    freeMaze(m);
}

/* --- Tests pour displayMaze --- */

MU_TEST(test_displayMaze_execution) {
    displayMaze(NULL);
    Maze empty = {0};
    displayMaze(&empty);

    Maze* m = createMaze(5, 5);
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            m->tiles[y][x] = ((x == 1) || (y == 1)) ? PATH : WALL;
        }
    }
    displayMaze(m);
    mu_check(true);
    freeMaze(m);
}

/* --- Suite de tests du module Maze --- */

MU_TEST_SUITE(maze_test_suite) {
    MU_RUN_TEST(test_areValidDimensions_valid);
    MU_RUN_TEST(test_areValidDimensions_even_numbers);
    MU_RUN_TEST(test_areValidDimensions_below_minimum);
    MU_RUN_TEST(test_createMaze_success);
    MU_RUN_TEST(test_createMaze_invalid_dimensions);
    MU_RUN_TEST(test_freeMaze_null_pointer);
    MU_RUN_TEST(test_initMaze_and_freeMazeGrid);
    MU_RUN_TEST(test_freeMazeGrid_null_pointer);
    MU_RUN_TEST(test_isValidMove_null_maze);
    MU_RUN_TEST(test_isValidMove_out_of_bounds);
    MU_RUN_TEST(test_isValidMove_tile_types);
    MU_RUN_TEST(test_movePlayer_null_maze);
    MU_RUN_TEST(test_movePlayer_valid_movement);
    MU_RUN_TEST(test_movePlayer_blocked_by_wall);
    MU_RUN_TEST(test_movePlayer_blocked_by_bounds);
    MU_RUN_TEST(test_movePlayerDirection_null_maze);
    MU_RUN_TEST(test_movePlayerDirection_cardinal_directions);
    MU_RUN_TEST(test_movePlayerDirection_uppercase_support);
    MU_RUN_TEST(test_movePlayerDirection_invalid_keys);
    MU_RUN_TEST(test_getChar_null_and_out_of_bounds);
    MU_RUN_TEST(test_getChar_player_position);
    MU_RUN_TEST(test_getChar_tile_symbols);
    MU_RUN_TEST(test_isMazeSolved_logic);
    MU_RUN_TEST(test_displayMaze_execution);
}

int run_maze_tests(void) {
    MU_RUN_SUITE(maze_test_suite);
    MU_REPORT();
    return minunit_fail;
}
