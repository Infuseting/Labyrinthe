/**
 * @file test_maze.c
 * @brief Tests unitaires du module maze sous MinUnit.
 * @author Serret Arthur
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "minunit.h"

/**
 * @brief Compte le nombre de murs intérieurs ouverts dans le labyrinthe.
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @return Nombre de murs intérieurs ouverts.
 */
static int count_open_internal_walls(const Maze *maze) {
    int num_cell_rows = (maze->height - 1) / 2;
    int num_cell_cols = (maze->width - 1) / 2;
    int open_walls = 0;

    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols - 1; c++) {
            if (maze->grid[2 * r + 1][2 * c + 2] == EMPTY) {
                open_walls++;
            }
        }
    }
    for (int r = 0; r < num_cell_rows - 1; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            if (maze->grid[2 * r + 2][2 * c + 1] == EMPTY) {
                open_walls++;
            }
        }
    }
    return open_walls;
}

/**
 * @brief Vérifie la connectivité complète par BFS depuis (1, 1).
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @return Nombre de cellules atteintes, ou valeur négative si entrée/sortie non atteintes.
 */
static int count_reachable_cells(const Maze *maze) {
    int num_cell_rows = (maze->height - 1) / 2;
    int num_cell_cols = (maze->width - 1) / 2;

    int **visited = malloc((size_t)maze->height * sizeof(int *));
    if (!visited) {
        return -1;
    }
    for (int r = 0; r < maze->height; r++) {
        visited[r] = calloc((size_t)maze->width, sizeof(int));
        if (!visited[r]) {
            for (int prev = 0; prev < r; prev++) {
                free(visited[prev]);
            }
            free(visited);
            return -1;
        }
    }

    int max_queue = maze->height * maze->width;
    int *q_r = malloc((size_t)max_queue * sizeof(int));
    int *q_c = malloc((size_t)max_queue * sizeof(int));
    if (!q_r || !q_c) {
        free(q_r);
        free(q_c);
        for (int r = 0; r < maze->height; r++) {
            free(visited[r]);
        }
        free(visited);
        return -1;
    }

    int head = 0;
    int tail = 0;

    q_r[tail] = 1;
    q_c[tail] = 1;
    tail++;
    visited[1][1] = 1;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (head < tail) {
        int cr = q_r[head];
        int cc = q_c[head];
        head++;

        for (int i = 0; i < 4; i++) {
            int nr = cr + dr[i];
            int nc = cc + dc[i];

            if (nr >= 0 && nr < maze->height && nc >= 0 && nc < maze->width) {
                if (!visited[nr][nc] && maze->grid[nr][nc] != WALL) {
                    visited[nr][nc] = 1;
                    q_r[tail] = nr;
                    q_c[tail] = nc;
                    tail++;
                }
            }
        }
    }

    int reached = 0;
    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            if (visited[2 * r + 1][2 * c + 1]) {
                reached++;
            }
        }
    }

    int player_reached = visited[maze->player.y][maze->player.x];
    int exit_reached = visited[maze->exit.y][maze->exit.x];

    for (int r = 0; r < maze->height; r++) {
        free(visited[r]);
    }
    free(visited);
    free(q_r);
    free(q_c);

    if (!player_reached || !exit_reached) {
        return -2;
    }

    return reached;
}

/**
 * @brief Cas nominal : création et génération d'un labyrinthe 11x25.
 */
MU_TEST(test_cas_nominal_creation_et_generation_11x25) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(11, maze->height);
    mu_assert_int_eq(25, maze->width);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);
    mu_assert_int_eq(23, maze->exit.x);
    mu_assert_int_eq(10, maze->exit.y);

    mu_assert_int_eq(0, maze_generate(maze));

    int num_cell_rows = (DEFAULT_HEIGHT - 1) / 2;
    int num_cell_cols = (DEFAULT_WIDTH - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    mu_assert_int_eq(total_cells - 1, count_open_internal_walls(maze));
    mu_assert_int_eq(total_cells, count_reachable_cells(maze));

    for (int c = 0; c < DEFAULT_WIDTH; c++) {
        if (c != maze->player.x) {
            mu_check(maze->grid[0][c] == WALL);
        }
        if (c != maze->exit.x) {
            mu_check(maze->grid[DEFAULT_HEIGHT - 1][c] == WALL);
        }
    }

    for (int r = 0; r < DEFAULT_HEIGHT; r++) {
        mu_check(maze->grid[r][0] == WALL);
        mu_check(maze->grid[r][DEFAULT_WIDTH - 1] == WALL);
    }

    maze_free(maze);
}

/**
 * @brief Cas nominal : validation sur la taille d'exemple 5x7 de l'annexe.
 */
MU_TEST(test_cas_nominal_annexe_5x7) {
    Maze *maze = maze_create(5, 7);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    int total_cells = 2 * 3;
    mu_assert_int_eq(total_cells - 1, count_open_internal_walls(maze));
    mu_assert_int_eq(total_cells, count_reachable_cells(maze));

    maze_free(maze);
}

/**
 * @brief Cas nominal : sauvegarde et rechargement fidèle d'un labyrinthe en fichier .cfg.
 */
MU_TEST(test_cas_nominal_sauvegarde_et_chargement) {
    const char *test_path = "test_save_load.cfg";
    Maze *original = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(original != NULL);
    mu_assert_int_eq(0, maze_generate(original));

    mu_assert_int_eq(0, maze_save(original, test_path));

    Maze *loaded = maze_load(test_path);
    mu_check(loaded != NULL);
    mu_assert_int_eq(original->height, loaded->height);
    mu_assert_int_eq(original->width, loaded->width);
    mu_assert_int_eq(original->player.x, loaded->player.x);
    mu_assert_int_eq(original->player.y, loaded->player.y);
    mu_assert_int_eq(original->exit.x, loaded->exit.x);
    mu_assert_int_eq(original->exit.y, loaded->exit.y);

    for (int r = 0; r < original->height; r++) {
        for (int c = 0; c < original->width; c++) {
            mu_assert_int_eq((int)original->grid[r][c], (int)loaded->grid[r][c]);
        }
    }

    maze_free(original);
    maze_free(loaded);
    remove(test_path);
}

/**
 * @brief Cas nominal : déplacement valide du joueur dans les couloirs ouverts.
 */
MU_TEST(test_cas_nominal_deplacement_joueur) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);

    bool moved = maze_move(maze, 0, 1);
    mu_check(moved == true);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(1, maze->player.y);

    maze_free(maze);
}

/**
 * @brief Cas nominal : mémorisation et plafonnement de la traînée aux 10 derniers mouvements.
 */
MU_TEST(test_cas_nominal_trainee_10_positions) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_assert_int_eq(0, maze->trail_count);

    mu_check(maze_move(maze, 0, 1) == true);
    mu_assert_int_eq(1, maze->trail_count);
    mu_assert_int_eq(1, maze->trail[0].x);
    mu_assert_int_eq(0, maze->trail[0].y);

    mu_check(maze_move(maze, 0, -1) == true);
    mu_assert_int_eq(2, maze->trail_count);
    mu_assert_int_eq(1, maze->trail[0].x);
    mu_assert_int_eq(1, maze->trail[0].y);
    mu_assert_int_eq(1, maze->trail[1].x);
    mu_assert_int_eq(0, maze->trail[1].y);

    for (int i = 0; i < 15; i++) {
        if (maze->player.y == 0) {
            maze_move(maze, 0, 1);
        } else {
            maze_move(maze, 0, -1);
        }
    }
    mu_assert_int_eq(MAX_TRAIL, maze->trail_count);

    maze_reset_trail(maze);
    mu_assert_int_eq(0, maze->trail_count);

    maze_free(maze);
}

/**
 * @brief Cas limite : taille minimale valide 3x3 (1 seule case).
 */
MU_TEST(test_cas_limite_taille_minimale_3x3) {
    Maze *maze = maze_create(3, 3);
    mu_check(maze != NULL);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);
    mu_assert_int_eq(1, maze->exit.x);
    mu_assert_int_eq(2, maze->exit.y);

    mu_assert_int_eq(0, maze_generate(maze));
    mu_assert_int_eq(0, count_open_internal_walls(maze));
    mu_assert_int_eq(1, count_reachable_cells(maze));

    maze_free(maze);
}

/**
 * @brief Cas limite : labyrinthe couloir horizontal (hauteur 3, largeur 25).
 */
MU_TEST(test_cas_limite_couloir_horizontal_3x25) {
    Maze *maze = maze_create(3, 25);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    int total_cells = 1 * 12;
    mu_assert_int_eq(total_cells - 1, count_open_internal_walls(maze));
    mu_assert_int_eq(total_cells, count_reachable_cells(maze));

    maze_free(maze);
}

/**
 * @brief Cas limite : labyrinthe couloir vertical (hauteur 11, largeur 3).
 */
MU_TEST(test_cas_limite_couloir_vertical_11x3) {
    Maze *maze = maze_create(11, 3);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    int total_cells = 5 * 1;
    mu_assert_int_eq(total_cells - 1, count_open_internal_walls(maze));
    mu_assert_int_eq(total_cells, count_reachable_cells(maze));

    maze_free(maze);
}

/**
 * @brief Cas limite : blocage du déplacement contre un mur et sortie de grille.
 */
MU_TEST(test_cas_limite_collisions_murs_et_bordures) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);

    mu_check(maze_move(maze, 0, -1) == false);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);

    mu_check(maze_move(maze, -1, 0) == false);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);

    mu_check(maze_move(maze, 1, 0) == false);
    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);

    maze_free(maze);
}

/**
 * @brief Cas limite : détection exacte de l'arrivée sur la sortie.
 */
MU_TEST(test_cas_limite_detection_sortie) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_check(maze_is_exit_reached(maze) == false);

    maze->player.x = maze->exit.x;
    maze->player.y = maze->exit.y;
    mu_check(maze_is_exit_reached(maze) == true);

    maze_free(maze);
}

/**
 * @brief Cas limite : libération d'un pointeur NULL sans plantage.
 */
MU_TEST(test_cas_limite_free_null) {
    maze_free(NULL);
    maze_reset_trail(NULL);
    mu_check(true);
}

/**
 * @brief Cas aléatoire : variabilité et conformité sur plusieurs exécutions.
 */
MU_TEST(test_cas_aleatoire_variabilite_et_validite_multiples) {
    Maze *m1 = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    Maze *m2 = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(m1 != NULL && m2 != NULL);

    int differences_found = 0;
    int total_cells = ((DEFAULT_HEIGHT - 1) / 2) * ((DEFAULT_WIDTH - 1) / 2);

    for (int run = 0; run < 10; run++) {
        mu_assert_int_eq(0, maze_generate(m1));
        mu_assert_int_eq(0, maze_generate(m2));

        mu_assert_int_eq(total_cells, count_reachable_cells(m1));
        mu_assert_int_eq(total_cells, count_reachable_cells(m2));

        for (int r = 0; r < DEFAULT_HEIGHT; r++) {
            for (int c = 0; c < DEFAULT_WIDTH; c++) {
                if (m1->grid[r][c] != m2->grid[r][c]) {
                    differences_found++;
                }
            }
        }
    }

    mu_check(differences_found > 0);

    maze_free(m1);
    maze_free(m2);
}

/**
 * @brief Cas d'erreur : rejet des dimensions paires.
 */
MU_TEST(test_cas_erreur_dimensions_paires) {
    mu_check(maze_create(10, 25) == NULL);
    mu_check(maze_create(11, 24) == NULL);
    mu_check(maze_create(10, 24) == NULL);
    mu_check(maze_create(4, 4) == NULL);
}

/**
 * @brief Cas d'erreur : rejet des dimensions inférieures à 3 ou négatives.
 */
MU_TEST(test_cas_erreur_dimensions_trop_petites_ou_negatives) {
    mu_check(maze_create(1, 25) == NULL);
    mu_check(maze_create(11, 1) == NULL);
    mu_check(maze_create(1, 1) == NULL);
    mu_check(maze_create(0, 0) == NULL);
    mu_check(maze_create(-11, 25) == NULL);
    mu_check(maze_create(11, -25) == NULL);
    mu_check(maze_create(-5, -5) == NULL);
}

/**
 * @brief Cas d'erreur : gestion des erreurs sur fichiers de sauvegarde et chargement.
 */
MU_TEST(test_cas_erreur_fichiers_sauvegarde_et_chargement) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);

    mu_assert_int_eq(-1, maze_save(NULL, "test.cfg"));
    mu_assert_int_eq(-1, maze_save(maze, NULL));
    mu_assert_int_eq(-1, maze_save(maze, "/invalid_directory_path/test.cfg"));

    mu_check(maze_load(NULL) == NULL);
    mu_check(maze_load("fichier_qui_n_existe_pas_123456789.cfg") == NULL);

    maze_free(maze);
}

/**
 * @brief Cas d'erreur : manipulation de pointeurs NULL sur génération, affichage et déplacement.
 */
MU_TEST(test_cas_erreur_pointeurs_null) {
    mu_assert_int_eq(-1, maze_generate(NULL));
    maze_print(NULL);
    mu_check(maze_move(NULL, 1, 0) == false);
    mu_check(maze_is_exit_reached(NULL) == false);
    mu_check(true);
}

/**
 * @brief Suite de tests unitaires pour le module labyrinthe.
 */
MU_TEST_SUITE(maze_test_suite) {
    MU_RUN_TEST(test_cas_nominal_creation_et_generation_11x25);
    MU_RUN_TEST(test_cas_nominal_annexe_5x7);
    MU_RUN_TEST(test_cas_nominal_sauvegarde_et_chargement);
    MU_RUN_TEST(test_cas_nominal_deplacement_joueur);
    MU_RUN_TEST(test_cas_nominal_trainee_10_positions);
    MU_RUN_TEST(test_cas_limite_taille_minimale_3x3);
    MU_RUN_TEST(test_cas_limite_couloir_horizontal_3x25);
    MU_RUN_TEST(test_cas_limite_couloir_vertical_11x3);
    MU_RUN_TEST(test_cas_limite_collisions_murs_et_bordures);
    MU_RUN_TEST(test_cas_limite_detection_sortie);
    MU_RUN_TEST(test_cas_limite_free_null);
    MU_RUN_TEST(test_cas_aleatoire_variabilite_et_validite_multiples);
    MU_RUN_TEST(test_cas_erreur_dimensions_paires);
    MU_RUN_TEST(test_cas_erreur_dimensions_trop_petites_ou_negatives);
    MU_RUN_TEST(test_cas_erreur_fichiers_sauvegarde_et_chargement);
    MU_RUN_TEST(test_cas_erreur_pointeurs_null);
}

/**
 * @brief Exécute la suite de tests unitaires du module maze.
 *
 * @return 0 si tous les tests ont réussi, nombre d'échecs sinon.
 */
int test_maze_run(void) {
    MU_RUN_SUITE(maze_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
