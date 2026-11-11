/**
 * @file maze.c
 * @brief Gestion et génération du labyrinthe.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maze.h"
#include "monster.h"

/**
 * @brief Structure représentant un mur intérieur entre deux cellules.
 */
typedef struct {
    int r1;       /* Ligne de la première cellule */
    int c1;       /* Colonne de la première cellule */
    int r2;       /* Ligne de la seconde cellule */
    int c2;       /* Colonne de la seconde cellule */
    int wall_row; /* Ligne du mur dans la grille d'affichage */
    int wall_col; /* Colonne du mur dans la grille d'affichage */
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
    maze->difficulty = DIFF_EASY;
    maze->player.x = 1;
    maze->player.y = 0;
    maze->exit.x = width - 2;
    maze->exit.y = height - 1;
    maze->has_key = false;
    maze->score = BASE_SCORE;
    maze->move_count = 0;
    maze->trail_count = 0;
    maze->monster_count = 0;
    maze->last_alert[0] = '\0';
    for (int i = 0; i < MAX_TRAIL; i++) {
        maze->trail[i].x = -1;
        maze->trail[i].y = -1;
    }

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

int maze_generate_with_difficulty(Maze *maze, Difficulty difficulty) {
    if (!maze || !maze->grid || maze->height < 3 || maze->width < 3 ||
        maze->height % 2 == 0 || maze->width % 2 == 0) {
        return -1;
    }

    maze->difficulty = difficulty;
    maze->monster_count = 0;
    maze->last_alert[0] = '\0';

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

    /* Mode difficile : ouvrir des murs intérieurs supplémentaires pour créer des cycles */
    if (difficulty == DIFF_HARD && remaining_walls > 0) {
        int extra_to_open = (total_cells - 1) / 5;
        if (extra_to_open < 1) {
            extra_to_open = 1;
        }
        if (extra_to_open > remaining_walls) {
            extra_to_open = remaining_walls;
        }

        for (int k = 0; k < extra_to_open; k++) {
            int r_idx = rand() % remaining_walls;
            Wall extra = walls[r_idx];
            walls[r_idx] = walls[remaining_walls - 1];
            remaining_walls--;
            maze->grid[extra.wall_row][extra.wall_col] = EMPTY;
        }
    }

    free(walls);
    free(cell_ids);

    maze->player.x = 1;
    maze->player.y = 0;
    maze->exit.x = maze->width - 2;
    maze->exit.y = maze->height - 1;
    maze->trail_count = 0;
    for (int i = 0; i < MAX_TRAIL; i++) {
        maze->trail[i].x = -1;
        maze->trail[i].y = -1;
    }

    maze->grid[maze->player.y][maze->player.x] = EMPTY;
    maze->grid[maze->exit.y][maze->exit.x] = EMPTY;

    maze_spawn_objects(maze, NUM_BONUSES, NUM_MALUSES);

    if (difficulty == DIFF_HARD) {
        maze_spawn_monsters(maze);
    }

    return 0;
}

int maze_generate(Maze *maze) {
    if (!maze) {
        return -1;
    }
    return maze_generate_with_difficulty(maze, maze->difficulty);
}

/**
 * @brief Vérifie si une coordonnée correspond à une position de la traînée du joueur.
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @param[in] r Ligne de la cellule.
 * @param[in] c Colonne de la cellule.
 * @return true si la position est dans la traînée, false sinon.
 */
static bool is_in_trail(const Maze *maze, int r, int c) {
    for (int i = 0; i < maze->trail_count; i++) {
        if (maze->trail[i].y == r && maze->trail[i].x == c) {
            return true;
        }
    }
    return false;
}

const Monster *maze_get_monster_at(const Maze *maze, int x, int y) {
    if (!maze) {
        return NULL;
    }

    for (int i = 0; i < maze->monster_count; i++) {
        if (maze->monsters[i].x == x && maze->monsters[i].y == y) {
            return &maze->monsters[i];
        }
    }
    return NULL;
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
                const Monster *m = maze_get_monster_at(maze, c, r);
                if (m) {
                    putchar((char)m->type);
                } else if (is_in_trail(maze, r, c)) {
                    fputs("·", stdout);
                } else {
                    putchar((char)maze->grid[r][c]);
                }
            }
        }
        putchar('\n');
    }
}

int maze_save(const Maze *maze, const char *filepath) {
    if (!maze || !maze->grid || !filepath) {
        return -1;
    }

    FILE *file = fopen(filepath, "w");
    if (!file) {
        return -1;
    }

    fprintf(file, "%d %d %d\n", maze->height, maze->width, (int)maze->difficulty);

    for (int r = 0; r < maze->height; r++) {
        for (int c = 0; c < maze->width; c++) {
            if (r == maze->player.y && c == maze->player.x) {
                fputc('o', file);
            } else if (r == maze->exit.y && c == maze->exit.x) {
                fputc('-', file);
            } else {
                const Monster *m = maze_get_monster_at(maze, c, r);
                if (m) {
                    fputc((char)m->type, file);
                } else {
                    fputc((char)maze->grid[r][c], file);
                }
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    return 0;
}

Maze *maze_load(const char *filepath) {
    if (!filepath) {
        return NULL;
    }

    FILE *file = fopen(filepath, "r");
    if (!file) {
        return NULL;
    }

    char header_line[128];
    if (!fgets(header_line, sizeof(header_line), file)) {
        fclose(file);
        return NULL;
    }

    int height = 0;
    int width = 0;
    int diff_int = DIFF_EASY;
    int parsed = sscanf(header_line, "%d %d %d", &height, &width, &diff_int);
    if (parsed < 2) {
        fclose(file);
        return NULL;
    }

    Maze *maze = maze_create(height, width);
    if (!maze) {
        fclose(file);
        return NULL;
    }

    if (parsed >= 3 && (diff_int == DIFF_EASY || diff_int == DIFF_HARD)) {
        maze->difficulty = (Difficulty)diff_int;
    }

    int key_count = 0;

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            int current_char = fgetc(file);
            if (current_char == '\r') {
                current_char = fgetc(file);
            }
            if (current_char == EOF || current_char == '\n') {
                maze_free(maze);
                fclose(file);
                return NULL;
            }

            if (current_char == 'o') {
                maze->player.x = c;
                maze->player.y = r;
                maze->grid[r][c] = EMPTY;
            } else if (current_char == '-') {
                maze->exit.x = c;
                maze->exit.y = r;
                maze->grid[r][c] = EMPTY;
            } else if (current_char == KEY) {
                maze->grid[r][c] = KEY;
                key_count++;
            } else if (current_char == BONUS) {
                maze->grid[r][c] = BONUS;
            } else if (current_char == MALUS) {
                maze->grid[r][c] = MALUS;
            } else if (current_char == MONSTER_GHOST) {
                if (maze->monster_count < MAX_MONSTERS) {
                    monster_init(&maze->monsters[maze->monster_count], MONSTER_GHOST, c, r);
                    maze->monster_count++;
                }
                maze->grid[r][c] = EMPTY;
                maze->difficulty = DIFF_HARD;
            } else if (current_char == MONSTER_OGRE) {
                if (maze->monster_count < MAX_MONSTERS) {
                    monster_init(&maze->monsters[maze->monster_count], MONSTER_OGRE, c, r);
                    maze->monster_count++;
                }
                maze->grid[r][c] = EMPTY;
                maze->difficulty = DIFF_HARD;
            } else if (current_char == WALL) {
                maze->grid[r][c] = WALL;
            } else {
                maze->grid[r][c] = EMPTY;
            }
        }

        int end_line = fgetc(file);
        if (end_line == '\r') {
            end_line = fgetc(file);
        }
        if (end_line != '\n' && end_line != EOF) {
            maze_free(maze);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);

    /* Fallback : si le labyrinthe charge ne comporte aucune cle (ex: labyrinthe issu
     * des etapes 1 ou 2), on place automatiquement une cle sur une case vide pour que
     * le joueur puisse deverrouiller la sortie. Si aucune case libre n'existe, on deverrouille
     * directement la porte (has_key = true). */
    if (key_count == 0) {
        int num_free = 0;
        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                if (maze->grid[r][c] == EMPTY &&
                    !(r == maze->player.y && c == maze->player.x) &&
                    !(r == maze->exit.y && c == maze->exit.x)) {
                    num_free++;
                }
            }
        }

        if (num_free > 0) {
            int target_idx = rand() % num_free;
            int cur_idx = 0;
            for (int r = 0; r < height; r++) {
                for (int c = 0; c < width; c++) {
                    if (maze->grid[r][c] == EMPTY &&
                        !(r == maze->player.y && c == maze->player.x) &&
                        !(r == maze->exit.y && c == maze->exit.x)) {
                        if (cur_idx == target_idx) {
                            maze->grid[r][c] = KEY;
                            cur_idx++;
                            break;
                        }
                        cur_idx++;
                    }
                }
                if (cur_idx > target_idx) {
                    break;
                }
            }
        } else {
            maze->has_key = true;
        }
    }

    return maze;
}

void maze_update_monsters(Maze *maze) {
    if (!maze) {
        return;
    }

    for (int i = 0; i < maze->monster_count; i++) {
        if (maze->monsters[i].move) {
            maze->monsters[i].move(&maze->monsters[i], maze);
        }

        /* Vérification si le monstre atteint la case du joueur */
        if (maze->monsters[i].x == maze->player.x && maze->monsters[i].y == maze->player.y) {
            int pen = maze->monsters[i].penalty_points;
            if (maze->score >= pen) {
                maze->score -= pen;
            } else {
                maze->score = 0;
            }

            if (maze->monsters[i].type == MONSTER_OGRE) {
                snprintf(maze->last_alert, sizeof(maze->last_alert),
                         "ATTAQUE ! Un Ogre vous a assomme (-%d pts) !", pen);
            } else {
                snprintf(maze->last_alert, sizeof(maze->last_alert),
                         "EFFROI ! Un Fantome vous a traverse (-%d pts) !", pen);
            }
        }
    }
}

bool maze_move(Maze *maze, int dx, int dy) {
    if (!maze || !maze->grid) {
        return false;
    }

    int next_x = maze->player.x + dx;
    int next_y = maze->player.y + dy;

    if (next_y < 0 || next_y >= maze->height || next_x < 0 || next_x >= maze->width) {
        return false;
    }

    if (maze->grid[next_y][next_x] == WALL) {
        return false;
    }

    if (next_x == maze->exit.x && next_y == maze->exit.y) {
        if (!maze->has_key) {
            return false;
        }
    }

    maze->last_alert[0] = '\0';

    if (maze->trail_count < MAX_TRAIL) {
        maze->trail_count++;
    }
    for (int i = maze->trail_count - 1; i > 0; i--) {
        maze->trail[i] = maze->trail[i - 1];
    }
    maze->trail[0] = maze->player;

    maze->player.x = next_x;
    maze->player.y = next_y;
    maze->move_count++;

    if (maze->score > MOVE_COST) {
        maze->score -= MOVE_COST;
    } else {
        maze->score = 0;
    }

    /* Traitement des objets ramassés */
    Cell cell_type = maze->grid[next_y][next_x];
    if (cell_type == KEY) {
        maze->has_key = true;
        maze->grid[next_y][next_x] = EMPTY;
    } else if (cell_type == BONUS) {
        maze->score += BONUS_POINTS;
        maze->grid[next_y][next_x] = EMPTY;
    } else if (cell_type == MALUS) {
        if (maze->score >= MALUS_POINTS) {
            maze->score -= MALUS_POINTS;
        } else {
            maze->score = 0;
        }
        maze->grid[next_y][next_x] = EMPTY;
    }

    /* Vérification si le joueur a marché sur un monstre */
    bool hit_on_step = false;
    const Monster *m_hit = maze_get_monster_at(maze, next_x, next_y);
    if (m_hit) {
        hit_on_step = true;
        int pen = m_hit->penalty_points;
        if (maze->score >= pen) {
            maze->score -= pen;
        } else {
            maze->score = 0;
        }
        if (m_hit->type == MONSTER_OGRE) {
            snprintf(maze->last_alert, sizeof(maze->last_alert),
                     "ATTAQUE ! Vous avez heurte un Ogre (-%d pts) !", pen);
        } else {
            snprintf(maze->last_alert, sizeof(maze->last_alert),
                     "EFFROI ! Vous avez traverse un Fantome (-%d pts) !", pen);
        }
    }

    /* Déplacement synchrone des monstres si la sortie n'est pas encore atteinte */
    if (!maze_is_exit_reached(maze)) {
        for (int i = 0; i < maze->monster_count; i++) {
            if (maze->monsters[i].move) {
                maze->monsters[i].move(&maze->monsters[i], maze);
            }

            if (!hit_on_step && maze->monsters[i].x == maze->player.x && maze->monsters[i].y == maze->player.y) {
                int pen = maze->monsters[i].penalty_points;
                if (maze->score >= pen) {
                    maze->score -= pen;
                } else {
                    maze->score = 0;
                }

                if (maze->monsters[i].type == MONSTER_OGRE) {
                    snprintf(maze->last_alert, sizeof(maze->last_alert),
                             "ATTAQUE ! Un Ogre vous a assomme (-%d pts) !", pen);
                } else {
                    snprintf(maze->last_alert, sizeof(maze->last_alert),
                             "EFFROI ! Un Fantome vous a traverse (-%d pts) !", pen);
                }
            }
        }
    }

    return true;
}

bool maze_wait(Maze *maze) {
    return maze_move(maze, 0, 0);
}

bool maze_is_exit_reached(const Maze *maze) {
    if (!maze) {
        return false;
    }

    return (maze->player.x == maze->exit.x && maze->player.y == maze->exit.y);
}

void maze_reset_trail(Maze *maze) {
    if (!maze) {
        return;
    }

    maze->trail_count = 0;
    for (int i = 0; i < MAX_TRAIL; i++) {
        maze->trail[i].x = -1;
        maze->trail[i].y = -1;
    }
}

void maze_spawn_objects(Maze *maze, int num_bonuses, int num_maluses) {
    if (!maze || !maze->grid) {
        return;
    }

    int num_cell_rows = (maze->height - 1) / 2;
    int num_cell_cols = (maze->width - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    Coords *free_cells = malloc((size_t)total_cells * sizeof(Coords));
    if (!free_cells) {
        return;
    }

    int free_count = 0;
    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            int gr = 2 * r + 1;
            int gc = 2 * c + 1;
            if ((gr == 1 && gc == 1) || (gr == maze->height - 2 && gc == maze->width - 2)) {
                continue;
            }
            if (maze->grid[gr][gc] == EMPTY) {
                free_cells[free_count].x = gc;
                free_cells[free_count].y = gr;
                free_count++;
            }
        }
    }

    for (int i = free_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Coords tmp = free_cells[i];
        free_cells[i] = free_cells[j];
        free_cells[j] = tmp;
    }

    int placed = 0;
    if (placed < free_count) {
        maze->grid[free_cells[placed].y][free_cells[placed].x] = KEY;
        placed++;
    }

    for (int b = 0; b < num_bonuses && placed < free_count; b++) {
        maze->grid[free_cells[placed].y][free_cells[placed].x] = BONUS;
        placed++;
    }

    for (int m = 0; m < num_maluses && placed < free_count; m++) {
        maze->grid[free_cells[placed].y][free_cells[placed].x] = MALUS;
        placed++;
    }

    free(free_cells);
}

void maze_spawn_monsters(Maze *maze) {
    if (!maze || !maze->grid) {
        return;
    }

    maze->monster_count = 0;

    int num_cell_rows = (maze->height - 1) / 2;
    int num_cell_cols = (maze->width - 1) / 2;
    int total_cells = num_cell_rows * num_cell_cols;

    Coords *free_cells = malloc((size_t)total_cells * sizeof(Coords));
    if (!free_cells) {
        return;
    }

    int free_count = 0;
    for (int r = 0; r < num_cell_rows; r++) {
        for (int c = 0; c < num_cell_cols; c++) {
            int gr = 2 * r + 1;
            int gc = 2 * c + 1;
            if ((gr == 1 && gc == 1) || (gr == maze->height - 2 && gc == maze->width - 2)) {
                continue;
            }
            if (maze->grid[gr][gc] == EMPTY) {
                free_cells[free_count].x = gc;
                free_cells[free_count].y = gr;
                free_count++;
            }
        }
    }

    for (int i = free_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Coords tmp = free_cells[i];
        free_cells[i] = free_cells[j];
        free_cells[j] = tmp;
    }

    int idx = 0;
    /* 1 Fantôme */
    if (idx < free_count && maze->monster_count < MAX_MONSTERS) {
        monster_init(&maze->monsters[maze->monster_count], MONSTER_GHOST,
                     free_cells[idx].x, free_cells[idx].y);
        maze->monster_count++;
        idx++;
    }

    /* 1 Ogre */
    if (idx < free_count && maze->monster_count < MAX_MONSTERS) {
        monster_init(&maze->monsters[maze->monster_count], MONSTER_OGRE,
                     free_cells[idx].x, free_cells[idx].y);
        maze->monster_count++;
        idx++;
    }

    free(free_cells);
}

void maze_reset_game_state(Maze *maze) {
    if (!maze) {
        return;
    }

    maze->player.x = 1;
    maze->player.y = 0;
    maze->has_key = false;
    maze->score = BASE_SCORE;
    maze->move_count = 0;
    maze->last_alert[0] = '\0';
    maze_reset_trail(maze);

    for (int i = 0; i < maze->monster_count; i++) {
        maze->monsters[i].x = maze->monsters[i].origin_x;
        maze->monsters[i].y = maze->monsters[i].origin_y;
    }
}
