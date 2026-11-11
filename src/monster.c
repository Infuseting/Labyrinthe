/**
 * @file monster.c
 * @brief Gestion des monstres et de leurs comportements via des pointeurs de fonction.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include "monster.h"
#include "maze.h"

/**
 * @brief Calcule la valeur absolue d'un entier.
 *
 * @param[in] n Valeur entière.
 * @return Valeur absolue de n.
 */
static int m_abs(int n) {
    return (n < 0) ? -n : n;
}

/**
 * @brief Vérifie si une coordonnée se trouve dans le territoire autorisé d'un monstre.
 *
 * @param[in] monster Pointeur vers le monstre.
 * @param[in] x Colonne cible.
 * @param[in] y Ligne cible.
 * @return true si la case est dans le territoire, false sinon.
 */
static bool is_within_territory(const Monster *monster, int x, int y) {
    int dx = m_abs(x - monster->origin_x);
    int dy = m_abs(y - monster->origin_y);
    return (dx <= monster->territory_radius && dy <= monster->territory_radius);
}

void monster_init(Monster *monster, MonsterType type, int x, int y) {
    if (!monster) {
        return;
    }

    monster->type = type;
    monster->x = x;
    monster->y = y;
    monster->origin_x = x;
    monster->origin_y = y;

    if (type == MONSTER_GHOST) {
        monster->territory_radius = GHOST_RADIUS;
        monster->penalty_points = GHOST_PENALTY;
        monster->move = monster_move_ghost;
    } else {
        monster->territory_radius = OGRE_RADIUS;
        monster->penalty_points = OGRE_PENALTY;
        monster->move = monster_move_ogre;
    }
}

void monster_move_ghost(Monster *monster, struct Maze *maze) {
    if (!monster || !maze || !maze->grid) {
        return;
    }

    int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    /* Si le joueur est dans le territoire du fantôme, le fantôme se dirige vers lui */
    if (is_within_territory(monster, maze->player.x, maze->player.y)) {
        int best_nx = monster->x;
        int best_ny = monster->y;
        int best_dist = m_abs(monster->x - maze->player.x) + m_abs(monster->y - maze->player.y);

        for (int i = 0; i < 4; i++) {
            int nx = monster->x + dirs[i][0];
            int ny = monster->y + dirs[i][1];

            /* Le fantôme traverse les murs intérieurs mais ne sort jamais de la grille */
            if (nx >= 1 && nx < maze->width - 1 && ny >= 1 && ny < maze->height - 1) {
                if (!(nx == maze->exit.x && ny == maze->exit.y) && is_within_territory(monster, nx, ny)) {
                    int dist = m_abs(nx - maze->player.x) + m_abs(ny - maze->player.y);
                    if (dist < best_dist) {
                        best_dist = dist;
                        best_nx = nx;
                        best_ny = ny;
                    }
                }
            }
        }

        monster->x = best_nx;
        monster->y = best_ny;
        return;
    }

    /* Déplacement aléatoire dans son territoire (traverse les murs) */
    int valid_dirs[4][2];
    int valid_count = 0;

    for (int i = 0; i < 4; i++) {
        int nx = monster->x + dirs[i][0];
        int ny = monster->y + dirs[i][1];

        if (nx >= 1 && nx < maze->width - 1 && ny >= 1 && ny < maze->height - 1) {
            if (!(nx == maze->exit.x && ny == maze->exit.y) && is_within_territory(monster, nx, ny)) {
                valid_dirs[valid_count][0] = dirs[i][0];
                valid_dirs[valid_count][1] = dirs[i][1];
                valid_count++;
            }
        }
    }

    if (valid_count > 0) {
        int choice = rand() % valid_count;
        monster->x += valid_dirs[choice][0];
        monster->y += valid_dirs[choice][1];
    }
}

void monster_move_ogre(Monster *monster, struct Maze *maze) {
    if (!monster || !maze || !maze->grid) {
        return;
    }

    int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    /* Recherche d'un trésor (BONUS) dans le territoire de l'ogre */
    int target_x = -1;
    int target_y = -1;
    int min_bonus_dist = 9999;

    for (int r = monster->origin_y - monster->territory_radius;
         r <= monster->origin_y + monster->territory_radius; r++) {
        for (int c = monster->origin_x - monster->territory_radius;
             c <= monster->origin_x + monster->territory_radius; c++) {
            if (r >= 0 && r < maze->height && c >= 0 && c < maze->width) {
                if (maze->grid[r][c] == BONUS && is_within_territory(monster, c, r)) {
                    int dist = m_abs(monster->x - c) + m_abs(monster->y - r);
                    if (dist < min_bonus_dist) {
                        min_bonus_dist = dist;
                        target_x = c;
                        target_y = r;
                    }
                }
            }
        }
    }

    /* Si aucun trésor n'est proche mais que le joueur est dans le territoire, cibler le joueur */
    if (target_x == -1 && is_within_territory(monster, maze->player.x, maze->player.y)) {
        target_x = maze->player.x;
        target_y = maze->player.y;
    }

    /* Si une cible existe, avancer vers elle sans traverser de murs */
    if (target_x != -1) {
        int best_nx = monster->x;
        int best_ny = monster->y;
        int best_dist = m_abs(monster->x - target_x) + m_abs(monster->y - target_y);

        for (int i = 0; i < 4; i++) {
            int nx = monster->x + dirs[i][0];
            int ny = monster->y + dirs[i][1];

            if (nx >= 1 && nx < maze->width - 1 && ny >= 1 && ny < maze->height - 1) {
                if (maze->grid[ny][nx] != WALL && !(nx == maze->exit.x && ny == maze->exit.y) &&
                    is_within_territory(monster, nx, ny)) {
                    int dist = m_abs(nx - target_x) + m_abs(ny - target_y);
                    if (dist < best_dist) {
                        best_dist = dist;
                        best_nx = nx;
                        best_ny = ny;
                    }
                }
            }
        }

        monster->x = best_nx;
        monster->y = best_ny;
        return;
    }

    /* Déplacement aléatoire dans les couloirs du territoire */
    int valid_dirs[4][2];
    int valid_count = 0;

    for (int i = 0; i < 4; i++) {
        int nx = monster->x + dirs[i][0];
        int ny = monster->y + dirs[i][1];

        if (nx >= 1 && nx < maze->width - 1 && ny >= 1 && ny < maze->height - 1) {
            if (maze->grid[ny][nx] != WALL && !(nx == maze->exit.x && ny == maze->exit.y) &&
                is_within_territory(monster, nx, ny)) {
                valid_dirs[valid_count][0] = dirs[i][0];
                valid_dirs[valid_count][1] = dirs[i][1];
                valid_count++;
            }
        }
    }

    if (valid_count > 0) {
        int choice = rand() % valid_count;
        monster->x += valid_dirs[choice][0];
        monster->y += valid_dirs[choice][1];
    }
}
