/**
 * @file game.c
 * @brief Boucle de jeu principale et gestion des actions du menu.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "game.h"
#include "maze.h"
#include "score.h"
#include "ui.h"

#define SAVES_DIR "saves"

/**
 * @brief Énumération locale représentant une direction de déplacement.
 */
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_QUIT,
    DIR_NONE
} Direction;

/**
 * @brief Assure la création du dossier de sauvegarde si inexistant.
 */
static void ensure_saves_dir(void) {
    if (mkdir(SAVES_DIR, 0755) != 0) {
        if (errno != EEXIST) {
            /* Le dossier existe déjà ou erreur gérée par les fonctions de fichier */
        }
    }
}

/**
 * @brief Construit le chemin complet du fichier .cfg dans le dossier saves/.
 *
 * @param[in] name Nom saisi par l'utilisateur.
 * @param[out] buffer Buffer recevant le chemin complet.
 * @param[in] max_len Taille maximale du buffer.
 */
static void build_filepath(const char *name, char *buffer, size_t max_len) {
    ensure_saves_dir();

    bool has_dir = (strstr(name, "/") != NULL);
    bool has_ext = false;
    size_t len = strlen(name);

    if (len >= 4 && strcmp(name + len - 4, ".cfg") == 0) {
        has_ext = true;
    }

    if (has_dir) {
        if (has_ext) {
            snprintf(buffer, max_len, "%s", name);
        } else {
            snprintf(buffer, max_len, "%s.cfg", name);
        }
    } else {
        if (has_ext) {
            snprintf(buffer, max_len, "%s/%s", SAVES_DIR, name);
        } else {
            snprintf(buffer, max_len, "%s/%s.cfg", SAVES_DIR, name);
        }
    }
}

/**
 * @brief Construit le chemin complet du fichier .score dans le dossier saves/.
 *
 * @param[in] name Nom du labyrinthe.
 * @param[out] buffer Buffer recevant le chemin complet.
 * @param[in] max_len Taille maximale du buffer.
 */
static void build_score_filepath(const char *name, char *buffer, size_t max_len) {
    ensure_saves_dir();

    const char *slash = strrchr(name, '/');
    const char *base = slash ? (slash + 1) : name;

    char clean_name[128];
    snprintf(clean_name, sizeof(clean_name), "%s", base);

    size_t len = strlen(clean_name);
    if (len >= 4 && strcmp(clean_name + len - 4, ".cfg") == 0) {
        clean_name[len - 4] = '\0';
    } else if (len >= 6 && strcmp(clean_name + len - 6, ".score") == 0) {
        clean_name[len - 6] = '\0';
    }

    snprintf(buffer, max_len, "%s/%s.score", SAVES_DIR, clean_name);
}

/**
 * @brief Convertit une touche du clavier en direction.
 *
 * @param[in] key Touche pressée.
 * @return Direction correspondante.
 */
static Direction key_to_direction(char key) {
    switch (key) {
        case 'z':
        case 'Z':
            return DIR_UP;
        case 's':
        case 'S':
            return DIR_DOWN;
        case 'q':
        case 'Q':
            return DIR_LEFT;
        case 'd':
        case 'D':
            return DIR_RIGHT;
        case 'c':
        case 'C':
        case 27:
            return DIR_QUIT;
        default:
            return DIR_NONE;
    }
}

/**
 * @brief Action du menu : Créer et enregistrer un labyrinthe.
 *
 * @param[in,out] current_maze Pointeur vers le labyrinthe actif.
 * @param[out] current_name Buffer recevant le nom du labyrinthe actif.
 * @param[in] max_name_len Taille maximale du buffer de nom.
 */
static void game_action_create(Maze **current_maze, char *current_name, size_t max_name_len) {
    char name[128];
    char filepath[256];

    printf("\n--- Creation d'un labyrinthe ---\n");
    if (!ui_prompt_string("Nom du labyrinthe : ", name, sizeof(name))) {
        ui_display_message("Creation annulee : nom invalide.");
        return;
    }

    int height = ui_prompt_odd_int("Hauteur (nombre impair >= 5) : ", MIN_MAZE_SIZE);
    int width = ui_prompt_odd_int("Largeur (nombre impair >= 5) : ", MIN_MAZE_SIZE);
    int diff_choice = ui_prompt_difficulty();
    Difficulty difficulty = (diff_choice == 2) ? DIFF_HARD : DIFF_EASY;

    Maze *new_maze = maze_create(height, width);
    if (!new_maze) {
        ui_display_message("Erreur : echec de l'allocation memoire du labyrinthe.");
        return;
    }

    if (maze_generate_with_difficulty(new_maze, difficulty) != 0) {
        maze_free(new_maze);
        ui_display_message("Erreur lors de la generation du labyrinthe.");
        return;
    }

    build_filepath(name, filepath, sizeof(filepath));
    if (maze_save(new_maze, filepath) != 0) {
        maze_free(new_maze);
        ui_display_message("Erreur : impossible d'enregistrer le fichier .cfg.");
        return;
    }

    if (*current_maze) {
        maze_free(*current_maze);
    }
    *current_maze = new_maze;
    snprintf(current_name, max_name_len, "%s", name);

    char msg[512];
    snprintf(msg, sizeof(msg), "Labyrinthe enregistre dans '%s' et automatiquement charge !", filepath);
    ui_display_message(msg);
}

/**
 * @brief Action du menu : Charger un labyrinthe existant.
 *
 * @param[in,out] current_maze Pointeur vers le labyrinthe actif.
 * @param[out] current_name Buffer recevant le nom du labyrinthe actif.
 * @param[in] max_name_len Taille maximale du buffer de nom.
 */
static void game_action_load(Maze **current_maze, char *current_name, size_t max_name_len) {
    char name[128];
    char filepath[256];

    ui_list_saved_mazes(SAVES_DIR);

    if (!ui_prompt_string("Nom du labyrinthe a charger : ", name, sizeof(name))) {
        ui_display_message("Chargement annule : nom invalide.");
        return;
    }

    build_filepath(name, filepath, sizeof(filepath));
    Maze *loaded = maze_load(filepath);
    if (!loaded) {
        char err[512];
        snprintf(err, sizeof(err), "Erreur : impossible de lire ou parser le fichier '%s'.", filepath);
        ui_display_message(err);
        return;
    }

    if (*current_maze) {
        maze_free(*current_maze);
    }
    *current_maze = loaded;
    snprintf(current_name, max_name_len, "%s", name);

    char msg[300];
    snprintf(msg, sizeof(msg), "Labyrinthe '%s' charge avec succes !", filepath);
    ui_display_message(msg);
}

/**
 * @brief Action du menu : Jouer une partie sur le labyrinthe courant.
 *
 * @param[in,out] current_maze Pointeur vers le pointeur du labyrinthe actif.
 * @param[in] maze_name Nom du labyrinthe actif.
 */
static void game_action_play(Maze **current_maze, const char *maze_name) {
    if (!current_maze || !*current_maze) {
        ui_display_message("Aucun labyrinthe charge. Veuillez d'abord en creer ou en charger un.");
        return;
    }

    if (maze_name && strlen(maze_name) > 0) {
        char filepath[256];
        build_filepath(maze_name, filepath, sizeof(filepath));
        Maze *fresh = maze_load(filepath);
        if (fresh) {
            maze_free(*current_maze);
            *current_maze = fresh;
        }
    }

    Maze *maze = *current_maze;
    maze_reset_game_state(maze);

    while (1) {
        ui_clear_screen();
        printf("Deplacez-vous avec Z (haut), Q (gauche), S (bas), D (droite).\n");
        printf("Appuyez sur C pour abandonner et revenir au menu.\n");

        ui_display_hud(maze->score, maze->move_count, maze->has_key,
                       (int)maze->difficulty, maze->last_alert);
        maze_print(maze);

        if (maze_is_exit_reached(maze)) {
            printf("\n=======================================================\n");
            printf("  BRAVO ! Vous avez atteint la sortie du labyrinthe !  \n");
            printf("  Score final : %d pts en %d deplacements\n", maze->score, maze->move_count);
            printf("=======================================================\n\n");

            if (maze_name && strlen(maze_name) > 0) {
                char score_path[256];
                build_score_filepath(maze_name, score_path, sizeof(score_path));

                ScoreBoard board;
                score_board_init(&board);
                score_board_load(&board, score_path);

                if (score_board_is_top10(&board, maze->score)) {
                    printf(" FELICITATIONS ! Votre score entre dans le Top 10 !\n");
                    char player_name[SCORE_NAME_MAX];
                    if (ui_prompt_string("Entrez votre pseudo/nom : ", player_name, sizeof(player_name))) {
                        score_board_add(&board, player_name, maze->score);
                        score_board_save(&board, score_path);
                        ui_display_scoreboard(&board, maze_name);
                    }
                }
            }
            ui_display_message(NULL);
            break;
        }

        char key = ui_get_key();
        while (key == '\n' || key == '\r' || key == ' ') {
            key = ui_get_key();
        }
        if (key == 0) {
            break;
        }
        Direction dir = key_to_direction(key);

        if (dir == DIR_QUIT) {
            break;
        }

        switch (dir) {
            case DIR_UP:
                maze_move(maze, 0, -1);
                break;
            case DIR_DOWN:
                maze_move(maze, 0, 1);
                break;
            case DIR_LEFT:
                maze_move(maze, -1, 0);
                break;
            case DIR_RIGHT:
                maze_move(maze, 1, 0);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Action du menu : Afficher le tableau des 10 meilleurs scores pour le labyrinthe actif.
 *
 * @param[in] maze Pointeur vers le labyrinthe actif.
 * @param[in] maze_name Nom du labyrinthe actif.
 */
static void game_action_show_scores(const Maze *maze, const char *maze_name) {
    if (!maze || !maze_name || strlen(maze_name) == 0) {
        ui_display_message("Aucun labyrinthe charge. Veuillez d'abord en creer ou en charger un.");
        return;
    }

    char score_path[256];
    build_score_filepath(maze_name, score_path, sizeof(score_path));

    ScoreBoard board;
    score_board_init(&board);
    score_board_load(&board, score_path);

    ui_display_scoreboard(&board, maze_name);
    ui_display_message(NULL);
}

void game_run(void) {
    Maze *current_maze = NULL;
    char current_maze_name[128] = {0};
    bool running = true;

    while (running) {
        ui_clear_screen();
        ui_display_menu(current_maze != NULL);

        int choice = ui_prompt_menu_choice();
        switch (choice) {
            case 1:
                game_action_create(&current_maze, current_maze_name, sizeof(current_maze_name));
                break;
            case 2:
                game_action_load(&current_maze, current_maze_name, sizeof(current_maze_name));
                break;
            case 3:
                game_action_play(&current_maze, current_maze_name);
                break;
            case 4:
                game_action_show_scores(current_maze, current_maze_name);
                break;
            case 5:
                running = false;
                break;
            default:
                ui_display_message("Choix invalide. Veuillez saisir un nombre entre 1 et 5.");
                break;
        }
    }

    if (current_maze) {
        maze_free(current_maze);
    }

    printf("\nAu revoir !\n");
}
