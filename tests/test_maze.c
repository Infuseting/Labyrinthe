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
 * @brief Cas nominal : présence des objets (clé, bonus, malus) et initialisation du score.
 */
MU_TEST(test_cas_nominal_objets_et_scoring) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    mu_assert_int_eq(BASE_SCORE, maze->score);
    mu_assert_int_eq(0, maze->move_count);
    mu_check(maze->has_key == false);

    int count_key = 0;
    int count_bonus = 0;
    int count_malus = 0;

    for (int r = 0; r < maze->height; r++) {
        for (int c = 0; c < maze->width; c++) {
            if (maze->grid[r][c] == KEY) {
                count_key++;
            } else if (maze->grid[r][c] == BONUS) {
                count_bonus++;
            } else if (maze->grid[r][c] == MALUS) {
                count_malus++;
            }
        }
    }

    mu_assert_int_eq(1, count_key);
    mu_assert_int_eq(NUM_BONUSES, count_bonus);
    mu_assert_int_eq(NUM_MALUSES, count_malus);

    maze_free(maze);
}

/**
 * @brief Cas nominal : ramassage des objets et impact arithmétique sur le score.
 */
MU_TEST(test_cas_nominal_interactions_objets) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);

    maze->grid[1][1] = EMPTY;
    maze->grid[1][2] = KEY;
    maze->grid[1][3] = BONUS;
    maze->grid[2][3] = MALUS;
    maze->player.x = 1;
    maze->player.y = 1;
    maze->score = BASE_SCORE;
    maze->has_key = false;

    /* Déplacement vers la clé (1, 2) */
    mu_check(maze_move(maze, 1, 0) == true);
    mu_check(maze->has_key == true);
    mu_check(maze->grid[1][2] == EMPTY);
    mu_assert_int_eq(BASE_SCORE - MOVE_COST, maze->score);

    /* Déplacement vers le bonus (1, 3) */
    mu_check(maze_move(maze, 1, 0) == true);
    mu_check(maze->grid[1][3] == EMPTY);
    mu_assert_int_eq(BASE_SCORE - 2 * MOVE_COST + BONUS_POINTS, maze->score);

    /* Déplacement vers le malus (2, 3) */
    mu_check(maze_move(maze, 0, 1) == true);
    mu_check(maze->grid[2][3] == EMPTY);
    mu_assert_int_eq(BASE_SCORE - 3 * MOVE_COST + BONUS_POINTS - MALUS_POINTS, maze->score);

    maze_free(maze);
}

/**
 * @brief Cas nominal : réinitialisation de l'état de jeu avec maze_reset_game_state.
 */
MU_TEST(test_cas_nominal_reinitialisation_etat_jeu) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);

    maze->player.x = 3;
    maze->player.y = 3;
    maze->has_key = true;
    maze->score = 420;
    maze->move_count = 17;
    maze->trail_count = 5;

    maze_reset_game_state(maze);

    mu_assert_int_eq(1, maze->player.x);
    mu_assert_int_eq(0, maze->player.y);
    mu_check(maze->has_key == false);
    mu_assert_int_eq(BASE_SCORE, maze->score);
    mu_assert_int_eq(0, maze->move_count);
    mu_assert_int_eq(0, maze->trail_count);

    maze_free(maze);
}

/**
 * @brief Cas nominal : sauvegarde et rechargement d'un labyrinthe contenant des objets dans le fichier .cfg.
 */
MU_TEST(test_cas_nominal_sauvegarde_chargement_objets) {
    const char *path = "saves/test_objects_cfg.cfg";
    Maze *m1 = maze_create(5, 5);
    mu_check(m1 != NULL);

    m1->grid[1][1] = KEY;
    m1->grid[1][2] = BONUS;
    m1->grid[1][3] = MALUS;

    mu_assert_int_eq(0, maze_save(m1, path));

    Maze *m2 = maze_load(path);
    mu_check(m2 != NULL);
    mu_check(m2->grid[1][1] == KEY);
    mu_check(m2->grid[1][2] == BONUS);
    mu_check(m2->grid[1][3] == MALUS);

    maze_free(m1);
    maze_free(m2);
    remove(path);
}

/**
 * @brief Cas limite : sortie verrouillée tant que la clé n'a pas été obtenue.
 */
MU_TEST(test_cas_limite_sortie_verrouillee_sans_cle) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);

    maze->exit.x = 3;
    maze->exit.y = 4;
    maze->grid[3][3] = EMPTY;
    maze->grid[4][3] = EMPTY;
    maze->player.x = 3;
    maze->player.y = 3;
    maze->has_key = false;

    /* Tente de sortir sans la clé : mouvement refusé */
    mu_check(maze_move(maze, 0, 1) == false);
    mu_assert_int_eq(3, maze->player.x);
    mu_assert_int_eq(3, maze->player.y);
    mu_check(maze_is_exit_reached(maze) == false);

    /* Avec la clé obtenue : mouvement accepté */
    maze->has_key = true;
    mu_check(maze_move(maze, 0, 1) == true);
    mu_assert_int_eq(3, maze->player.x);
    mu_assert_int_eq(4, maze->player.y);
    mu_check(maze_is_exit_reached(maze) == true);

    maze_free(maze);
}

/**
 * @brief Cas limite : le score ne descend jamais en dessous de 0 suite à malus ou déplacements.
 */
MU_TEST(test_cas_limite_score_plancher_zero) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);

    maze->grid[1][1] = EMPTY;
    maze->grid[1][2] = MALUS;
    maze->player.x = 1;
    maze->player.y = 1;
    maze->score = 50;

    mu_check(maze_move(maze, 1, 0) == true);
    mu_assert_int_eq(0, maze->score);

    /* Déplacement additionnel à score 0 reste à 0 */
    maze->grid[1][3] = EMPTY;
    mu_check(maze_move(maze, 1, 0) == true);
    mu_assert_int_eq(0, maze->score);

    maze_free(maze);
}

/**
 * @brief Cas limite : libération d'un pointeur NULL sans plantage.
 */
MU_TEST(test_cas_limite_free_null) {
    maze_free(NULL);
    maze_reset_trail(NULL);
    maze_reset_game_state(NULL);
    maze_spawn_objects(NULL, 0, 0);
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
 * @brief Cas nominal : génération en mode difficile avec chemins multiples et présence de monstres.
 */
MU_TEST(test_cas_nominal_generation_difficile_chemins_multiples) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate_with_difficulty(maze, DIFF_HARD));

    mu_assert_int_eq(DIFF_HARD, maze->difficulty);

    int num_cell_rows = (DEFAULT_HEIGHT - 1) / 2;
    int num_cell_cols = (DEFAULT_WIDTH - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    /* Chemins multiples : plus de murs ouverts qu'un arbre couvrant strict (total_cells - 1) */
    mu_check(count_open_internal_walls(maze) > (total_cells - 1));

    /* Présence des monstres */
    mu_check(maze->monster_count >= 2);
    bool has_ghost = false;
    bool has_ogre = false;
    for (int i = 0; i < maze->monster_count; i++) {
        if (maze->monsters[i].type == MONSTER_GHOST) has_ghost = true;
        if (maze->monsters[i].type == MONSTER_OGRE) has_ogre = true;
    }
    mu_check(has_ghost == true);
    mu_check(has_ogre == true);

    maze_free(maze);
}

/**
 * @brief Cas nominal : sauvegarde et rechargement d'un labyrinthe difficile avec ses monstres.
 */
MU_TEST(test_cas_nominal_sauvegarde_chargement_difficulte_et_monstres) {
    const char *save_path = "saves/test_save_hard.cfg";
    Maze *m1 = maze_create(7, 9);
    mu_check(m1 != NULL);
    mu_assert_int_eq(0, maze_generate_with_difficulty(m1, DIFF_HARD));
    mu_assert_int_eq(0, maze_save(m1, save_path));

    Maze *m2 = maze_load(save_path);
    mu_check(m2 != NULL);
    mu_assert_int_eq(DIFF_HARD, m2->difficulty);
    mu_check(m2->monster_count >= 2);

    maze_free(m1);
    maze_free(m2);
    remove(save_path);
}

/**
 * @brief Cas limite : le mode facile produit un arbre parfait sans aucun monstre.
 */
MU_TEST(test_cas_limite_mode_facile_sans_monstre) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate_with_difficulty(maze, DIFF_EASY));

    mu_assert_int_eq(DIFF_EASY, maze->difficulty);
    mu_assert_int_eq(0, maze->monster_count);

    int num_cell_rows = (DEFAULT_HEIGHT - 1) / 2;
    int num_cell_cols = (DEFAULT_WIDTH - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    mu_assert_int_eq(total_cells - 1, count_open_internal_walls(maze));

    maze_free(maze);
}

/**
 * @brief Cas limite : chargement d'un labyrinthe sans clé (étape 2) avec fallback automatique sur case vide.
 */
MU_TEST(test_cas_limite_chargement_fallback_sans_cle) {
    const char *path = "saves/test_legacy_no_key.cfg";
    FILE *f = fopen(path, "w");
    mu_check(f != NULL);
    /* Labyrinthe d'étape 2 sans clé, sans bonus, sans monstre */
    fprintf(f, "5 5\n");
    fprintf(f, "#o###\n");
    fprintf(f, "#   #\n");
    fprintf(f, "# # #\n");
    fprintf(f, "#   #\n");
    fprintf(f, "###-#\n");
    fclose(f);

    Maze *loaded = maze_load(path);
    mu_check(loaded != NULL);

    int key_found = 0;
    for (int r = 0; r < loaded->height; r++) {
        for (int c = 0; c < loaded->width; c++) {
            if (loaded->grid[r][c] == KEY) {
                key_found++;
            }
        }
    }
    mu_assert_int_eq(1, key_found);

    maze_free(loaded);
    remove(path);
}

/**
 * @brief Cas limite : chargement d'un labyrinthe sans clé et sans case vide intermédiaire.
 */
MU_TEST(test_cas_limite_chargement_fallback_sans_cle_sans_case_libre) {
    const char *path = "saves/test_no_free.cfg";
    FILE *f = fopen(path, "w");
    mu_check(f != NULL);
    /* Labyrinthe 3x3 avec uniquement joueur et sortie, aucune case vide intermédiaire */
    fprintf(f, "3 3\n");
    fprintf(f, "#o#\n");
    fprintf(f, "###\n");
    fprintf(f, "#-#\n");
    fclose(f);

    Maze *loaded = maze_load(path);
    mu_check(loaded != NULL);
    mu_check(loaded->has_key == true);

    maze_free(loaded);
    remove(path);
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
    MU_RUN_TEST(test_cas_nominal_objets_et_scoring);
    MU_RUN_TEST(test_cas_nominal_interactions_objets);
    MU_RUN_TEST(test_cas_nominal_reinitialisation_etat_jeu);
    MU_RUN_TEST(test_cas_nominal_sauvegarde_chargement_objets);
    MU_RUN_TEST(test_cas_nominal_generation_difficile_chemins_multiples);
    MU_RUN_TEST(test_cas_nominal_sauvegarde_chargement_difficulte_et_monstres);
    MU_RUN_TEST(test_cas_limite_taille_minimale_3x3);
    MU_RUN_TEST(test_cas_limite_couloir_horizontal_3x25);
    MU_RUN_TEST(test_cas_limite_couloir_vertical_11x3);
    MU_RUN_TEST(test_cas_limite_collisions_murs_et_bordures);
    MU_RUN_TEST(test_cas_limite_detection_sortie);
    MU_RUN_TEST(test_cas_limite_sortie_verrouillee_sans_cle);
    MU_RUN_TEST(test_cas_limite_chargement_fallback_sans_cle);
    MU_RUN_TEST(test_cas_limite_chargement_fallback_sans_cle_sans_case_libre);
    MU_RUN_TEST(test_cas_limite_score_plancher_zero);
    MU_RUN_TEST(test_cas_limite_mode_facile_sans_monstre);
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
