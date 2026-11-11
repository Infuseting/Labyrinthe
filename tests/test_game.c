/**
 * @file test_game.c
 * @brief Tests unitaires du module logique de jeu (game.h).
 * @author Serret Arthur
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "maze.h"
#include "minunit.h"

/**
 * @brief Cas nominal : résolution complète d'une partie avec déplacement pas-à-pas jusqu'à la sortie.
 */
MU_TEST(test_game_nominal_partie_resolue_avec_succes) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    int parent_r[11][25];
    int parent_c[11][25];
    int visited[11][25] = {{0}};

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 25; c++) {
            parent_r[r][c] = -1;
            parent_c[r][c] = -1;
        }
    }

    int q_r[300];
    int q_c[300];
    int head = 0;
    int tail = 0;

    q_r[tail] = maze->player.y;
    q_c[tail] = maze->player.x;
    tail++;
    visited[maze->player.y][maze->player.x] = 1;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (head < tail) {
        int cr = q_r[head];
        int cc = q_c[head];
        head++;

        if (cr == maze->exit.y && cc == maze->exit.x) {
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nr = cr + dr[i];
            int nc = cc + dc[i];

            if (nr >= 0 && nr < maze->height && nc >= 0 && nc < maze->width) {
                if (!visited[nr][nc] && maze->grid[nr][nc] != WALL) {
                    visited[nr][nc] = 1;
                    parent_r[nr][nc] = cr;
                    parent_c[nr][nc] = cc;
                    q_r[tail] = nr;
                    q_c[tail] = nc;
                    tail++;
                }
            }
        }
    }

    int path_r[300];
    int path_c[300];
    int path_len = 0;

    int curr_r = maze->exit.y;
    int curr_c = maze->exit.x;

    while (curr_r != -1 && curr_c != -1) {
        path_r[path_len] = curr_r;
        path_c[path_len] = curr_c;
        path_len++;

        if (curr_r == maze->player.y && curr_c == maze->player.x) {
            break;
        }

        int pr = parent_r[curr_r][curr_c];
        int pc = parent_c[curr_r][curr_c];
        curr_r = pr;
        curr_c = pc;
    }

    mu_check(path_len > 1);

    for (int i = path_len - 2; i >= 0; i--) {
        int dx = path_c[i] - maze->player.x;
        int dy = path_r[i] - maze->player.y;
        if (i == 0) {
            /* Avant d'atteindre la sortie, vérifier que sans clé le passage est bloqué */
            maze->has_key = false;
            mu_check(maze_move(maze, dx, dy) == false);
            maze->has_key = true;
        }
        bool ok = maze_move(maze, dx, dy);
        mu_check(ok == true);
    }

    mu_check(maze_is_exit_reached(maze) == true);
    maze_free(maze);
}

/**
 * @brief Cas nominal : sauvegarde, chargement et rejouabilité d'une session de jeu.
 */
MU_TEST(test_game_nominal_chargement_et_rejouabilite) {
    const char *save_file = "saves/test_game_replay.cfg";
    Maze *m1 = maze_create(7, 9);
    mu_check(m1 != NULL);
    mu_assert_int_eq(0, maze_generate(m1));
    mu_assert_int_eq(0, maze_save(m1, save_file));

    Maze *m2 = maze_load(save_file);
    mu_check(m2 != NULL);
    mu_assert_int_eq(1, m2->player.x);
    mu_assert_int_eq(0, m2->player.y);

    mu_check(maze_move(m2, 0, 1) == true);
    mu_assert_int_eq(1, m2->player.x);
    mu_assert_int_eq(1, m2->player.y);

    m2->player.x = 1;
    m2->player.y = 0;
    mu_assert_int_eq(1, m2->player.x);
    mu_assert_int_eq(0, m2->player.y);

    maze_free(m1);
    maze_free(m2);
    remove(save_file);
}

/**
 * @brief Cas limite : résolution du plus petit labyrinthe possible (3x3).
 */
MU_TEST(test_game_limite_resolution_3x3) {
    Maze *maze = maze_create(3, 3);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_check(maze_is_exit_reached(maze) == false);
    mu_check(maze_move(maze, 0, 1) == true);
    mu_check(maze_is_exit_reached(maze) == false);

    /* Sortie verrouillée sans clé */
    mu_check(maze_move(maze, 0, 1) == false);

    /* Avec clé */
    maze->has_key = true;
    mu_check(maze_move(maze, 0, 1) == true);
    mu_check(maze_is_exit_reached(maze) == true);

    maze_free(maze);
}

/**
 * @brief Cas limite : abandon de partie en cours sans atteindre la sortie.
 */
MU_TEST(test_game_limite_abandon_partie) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    maze_move(maze, 0, 1);
    mu_check(maze_is_exit_reached(maze) == false);

    maze_free(maze);
}

/**
 * @brief Cas aléatoire : résolution automatique sur plusieurs tailles de labyrinthes aléatoires.
 */
MU_TEST(test_game_aleatoire_resolutions_multiples) {
    int test_sizes[4][2] = {{5, 7}, {7, 9}, {9, 11}, {11, 25}};

    for (int t = 0; t < 4; t++) {
        int h = test_sizes[t][0];
        int w = test_sizes[t][1];

        Maze *m = maze_create(h, w);
        mu_check(m != NULL);
        mu_assert_int_eq(0, maze_generate(m));

        mu_check(maze_move(m, 0, 1) == true);
        mu_check(m->player.y == 1);
        mu_check(m->player.x == 1);

        maze_free(m);
    }
}

/**
 * @brief Cas d'erreur : chargement d'un fichier corrompu (dimensions invalides ou contenu tronqué).
 */
MU_TEST(test_game_erreur_chargement_fichier_corrompu) {
    const char *corrupt_path = "saves/test_corrupt.cfg";

    FILE *f1 = fopen(corrupt_path, "w");
    if (f1) {
        fprintf(f1, "10 24\n# #\n");
        fclose(f1);
    }
    mu_check(maze_load(corrupt_path) == NULL);

    FILE *f2 = fopen(corrupt_path, "w");
    if (f2) {
        fprintf(f2, "abc def\n");
        fclose(f2);
    }
    mu_check(maze_load(corrupt_path) == NULL);

    remove(corrupt_path);
}

/**
 * @brief Cas nominal : déroulement d'une session de jeu en mode difficile avec monstres actifs.
 */
MU_TEST(test_game_nominal_partie_difficile_avec_monstres) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate_with_difficulty(maze, DIFF_HARD));
    mu_assert_int_eq(DIFF_HARD, maze->difficulty);
    mu_check(maze->monster_count >= 2);

    /* Déplacement initial du joueur dans le labyrinthe */
    mu_check(maze_move(maze, 0, 1) == true);
    mu_assert_int_eq(1, maze->move_count);

    maze_free(maze);
}

/**
 * @brief Cas d'erreur : vérification de l'intégrité face à un labyrinthe NULL.
 */
MU_TEST(test_game_erreur_pointeurs_null) {
    mu_check(maze_move(NULL, 0, 1) == false);
    mu_check(maze_is_exit_reached(NULL) == false);
}

/**
 * @brief Suite de tests pour le module game.
 */
MU_TEST_SUITE(game_test_suite) {
    MU_RUN_TEST(test_game_nominal_partie_resolue_avec_succes);
    MU_RUN_TEST(test_game_nominal_chargement_et_rejouabilite);
    MU_RUN_TEST(test_game_nominal_partie_difficile_avec_monstres);
    MU_RUN_TEST(test_game_limite_resolution_3x3);
    MU_RUN_TEST(test_game_limite_abandon_partie);
    MU_RUN_TEST(test_game_aleatoire_resolutions_multiples);
    MU_RUN_TEST(test_game_erreur_chargement_fichier_corrompu);
    MU_RUN_TEST(test_game_erreur_pointeurs_null);
}

/**
 * @brief Exécute la suite de tests unitaires du module game.
 *
 * @return 0 si tous les tests ont réussi, nombre d'échecs sinon.
 */
int test_game_run(void) {
    MU_RUN_SUITE(game_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
