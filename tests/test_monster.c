/**
 * @file test_monster.c
 * @brief Tests unitaires du module des monstres et comportements (monster.h).
 * @author Serret Arthur
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monster.h"
#include "maze.h"
#include "minunit.h"

/**
 * @brief Calcule la valeur absolue d'un entier.
 *
 * @param[in] n Valeur entière.
 * @return Valeur absolue de n.
 */
static int test_m_abs(int n) {
    return (n < 0) ? -n : n;
}

/**
 * @brief Cas nominal : initialisation d'un fantôme et d'un ogre et conformité des règles.
 */
MU_TEST(test_monster_nominal_initialisation_fantome_et_ogre) {
    Monster ghost;
    monster_init(&ghost, MONSTER_GHOST, 5, 7);
    mu_assert_int_eq(MONSTER_GHOST, ghost.type);
    mu_assert_int_eq(5, ghost.x);
    mu_assert_int_eq(7, ghost.y);
    mu_assert_int_eq(5, ghost.origin_x);
    mu_assert_int_eq(7, ghost.origin_y);
    mu_assert_int_eq(GHOST_RADIUS, ghost.territory_radius);
    mu_assert_int_eq(GHOST_PENALTY, ghost.penalty_points);
    mu_check(ghost.move == monster_move_ghost);

    Monster ogre;
    monster_init(&ogre, MONSTER_OGRE, 3, 3);
    mu_assert_int_eq(MONSTER_OGRE, ogre.type);
    mu_assert_int_eq(3, ogre.x);
    mu_assert_int_eq(3, ogre.y);
    mu_assert_int_eq(3, ogre.origin_x);
    mu_assert_int_eq(3, ogre.origin_y);
    mu_assert_int_eq(OGRE_RADIUS, ogre.territory_radius);
    mu_assert_int_eq(OGRE_PENALTY, ogre.penalty_points);
    mu_check(ogre.move == monster_move_ogre);

    /* Règle du sujet : le territoire est d'autant plus grand que les pénalités sont fortes */
    mu_check(ogre.penalty_points > ghost.penalty_points);
    mu_check(ogre.territory_radius > ghost.territory_radius);
}

/**
 * @brief Cas nominal : le fantôme traverse les murs intérieurs via son pointeur de fonction.
 */
MU_TEST(test_monster_nominal_deplacement_fantome_traverse_murs) {
    Maze *maze = maze_create(7, 7);
    mu_check(maze != NULL);

    /* Remplir l'intérieur de murs pour prouver que le fantôme passe à travers */
    for (int r = 1; r < 6; r++) {
        for (int c = 1; c < 6; c++) {
            maze->grid[r][c] = WALL;
        }
    }

    maze->player.x = 2;
    maze->player.y = 2;

    Monster ghost;
    monster_init(&ghost, MONSTER_GHOST, 3, 3);
    mu_check(ghost.move != NULL);

    /* Déplacement du fantôme via son pointeur de fonction */
    ghost.move(&ghost, maze);

    /* Vérifier qu'il s'est déplacé vers le joueur malgré le mur */
    int dist = test_m_abs(ghost.x - maze->player.x) + test_m_abs(ghost.y - maze->player.y);
    mu_check(dist < 2);
    mu_check(maze->grid[ghost.y][ghost.x] == WALL);

    maze_free(maze);
}

/**
 * @brief Cas nominal : l'ogre ne franchit pas les murs et patrouille vers un trésor.
 */
MU_TEST(test_monster_nominal_deplacement_ogre_cible_tresor) {
    Maze *maze = maze_create(7, 7);
    mu_check(maze != NULL);

    /* Créer un couloir en ligne de (2, 2) à (2, 5) */
    for (int c = 2; c <= 5; c++) {
        maze->grid[2][c] = EMPTY;
    }
    maze->grid[2][5] = BONUS;

    Monster ogre;
    monster_init(&ogre, MONSTER_OGRE, 2, 2);
    mu_check(ogre.move != NULL);

    /* Déplacement de l'ogre */
    ogre.move(&ogre, maze);

    /* L'ogre a avancé vers le trésor sans traverser de murs */
    mu_assert_int_eq(2, ogre.y);
    mu_check(ogre.x >= 2);
    mu_check(maze->grid[ogre.y][ogre.x] != WALL);

    maze_free(maze);
}

/**
 * @brief Cas nominal : collision entre le joueur et un monstre et déduction des pénalités.
 */
MU_TEST(test_monster_nominal_collision_et_penalite) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);
    maze->grid[1][1] = EMPTY;
    maze->grid[1][2] = EMPTY;

    maze->player.x = 1;
    maze->player.y = 1;
    maze->score = BASE_SCORE;

    monster_init(&maze->monsters[0], MONSTER_OGRE, 2, 1);
    maze->monster_count = 1;

    /* Le joueur avance sur l'ogre en (2, 1) */
    mu_check(maze_move(maze, 1, 0) == true);

    /* Pénalité de l'ogre (150 pts) + coût du pas (1 pt) */
    mu_assert_int_eq(BASE_SCORE - MOVE_COST - OGRE_PENALTY, maze->score);
    mu_check(strlen(maze->last_alert) > 0);

    maze_free(maze);
}

/**
 * @brief Cas limite : confinement strict dans le territoire de mobilité.
 */
MU_TEST(test_monster_limite_confinement_territoire_mobilite) {
    Maze *maze = maze_create(11, 25);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate(maze));

    Monster ghost;
    monster_init(&ghost, MONSTER_GHOST, 5, 5);

    for (int t = 0; t < 50; t++) {
        ghost.move(&ghost, maze);
        int dx = test_m_abs(ghost.x - ghost.origin_x);
        int dy = test_m_abs(ghost.y - ghost.origin_y);
        mu_check(dx <= ghost.territory_radius);
        mu_check(dy <= ghost.territory_radius);
    }

    Monster ogre;
    monster_init(&ogre, MONSTER_OGRE, 5, 5);

    for (int t = 0; t < 50; t++) {
        ogre.move(&ogre, maze);
        int dx = test_m_abs(ogre.x - ogre.origin_x);
        int dy = test_m_abs(ogre.y - ogre.origin_y);
        mu_check(dx <= ogre.territory_radius);
        mu_check(dy <= ogre.territory_radius);
        mu_check(maze->grid[ogre.y][ogre.x] != WALL);
    }

    maze_free(maze);
}

/**
 * @brief Cas limite : respect absolu de l'enceinte extérieure du labyrinthe.
 */
MU_TEST(test_monster_limite_respect_enceinte_exterieure) {
    Maze *maze = maze_create(7, 7);
    mu_check(maze != NULL);

    Monster ghost;
    monster_init(&ghost, MONSTER_GHOST, 1, 1);

    for (int t = 0; t < 30; t++) {
        ghost.move(&ghost, maze);
        mu_check(ghost.x >= 1);
        mu_check(ghost.x < maze->width - 1);
        mu_check(ghost.y >= 1);
        mu_check(ghost.y < maze->height - 1);
    }

    maze_free(maze);
}

/**
 * @brief Cas limite : le score ne devient jamais négatif après une attaque de monstre.
 */
MU_TEST(test_monster_limite_score_plancher_zero) {
    Maze *maze = maze_create(5, 5);
    mu_check(maze != NULL);
    maze->grid[1][1] = EMPTY;
    maze->grid[1][2] = EMPTY;
    maze->player.x = 1;
    maze->player.y = 1;
    maze->score = 50;

    monster_init(&maze->monsters[0], MONSTER_OGRE, 2, 1);
    maze->monster_count = 1;

    mu_check(maze_move(maze, 1, 0) == true);
    mu_assert_int_eq(0, maze->score);

    maze_free(maze);
}

/**
 * @brief Cas aléatoire : simulation de 100 tours en mode difficile avec vérification des invariants.
 */
MU_TEST(test_monster_aleatoire_simulation_100_tours) {
    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    mu_check(maze != NULL);
    mu_assert_int_eq(0, maze_generate_with_difficulty(maze, DIFF_HARD));

    mu_assert_int_eq(DIFF_HARD, maze->difficulty);
    mu_check(maze->monster_count > 0);

    for (int step = 0; step < 100; step++) {
        maze_update_monsters(maze);

        for (int i = 0; i < maze->monster_count; i++) {
            const Monster *m = &maze->monsters[i];
            mu_check(m->x >= 1 && m->x < maze->width - 1);
            mu_check(m->y >= 1 && m->y < maze->height - 1);

            int dx = test_m_abs(m->x - m->origin_x);
            int dy = test_m_abs(m->y - m->origin_y);
            mu_check(dx <= m->territory_radius);
            mu_check(dy <= m->territory_radius);

            if (m->type == MONSTER_OGRE) {
                mu_check(maze->grid[m->y][m->x] != WALL);
            }
        }
    }

    maze_free(maze);
}

/**
 * @brief Cas d'erreur : manipulation de pointeurs NULL sur l'ensemble des fonctions du module.
 */
MU_TEST(test_monster_erreur_pointeurs_null) {
    Monster m;
    monster_init(NULL, MONSTER_GHOST, 1, 1);

    monster_move_ghost(NULL, NULL);
    monster_move_ghost(&m, NULL);

    monster_move_ogre(NULL, NULL);
    monster_move_ogre(&m, NULL);

    maze_update_monsters(NULL);
    mu_check(maze_get_monster_at(NULL, 1, 1) == NULL);
}

/**
 * @brief Suite de tests unitaires pour le module monster.
 */
MU_TEST_SUITE(monster_test_suite) {
    MU_RUN_TEST(test_monster_nominal_initialisation_fantome_et_ogre);
    MU_RUN_TEST(test_monster_nominal_deplacement_fantome_traverse_murs);
    MU_RUN_TEST(test_monster_nominal_deplacement_ogre_cible_tresor);
    MU_RUN_TEST(test_monster_nominal_collision_et_penalite);
    MU_RUN_TEST(test_monster_limite_confinement_territoire_mobilite);
    MU_RUN_TEST(test_monster_limite_respect_enceinte_exterieure);
    MU_RUN_TEST(test_monster_limite_score_plancher_zero);
    MU_RUN_TEST(test_monster_aleatoire_simulation_100_tours);
    MU_RUN_TEST(test_monster_erreur_pointeurs_null);
}

/**
 * @brief Exécute la suite de tests unitaires du module monster.
 *
 * @return 0 si tous les tests ont réussi, nombre d'échecs sinon.
 */
int test_monster_run(void) {
    MU_RUN_SUITE(monster_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
