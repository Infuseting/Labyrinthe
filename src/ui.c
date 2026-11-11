/**
 * @file ui.c
 * @brief Fonctions d'interaction utilisateur et d'affichage console.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include "ui.h"

void ui_clear_screen(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

void ui_display_menu(bool has_active_maze) {
    printf("\n========================================\n");
    printf("           JEU DU LABYRINTHE            \n");
    printf("========================================\n");
    if (has_active_maze) {
        printf(" [Statut : un labyrinthe est charge]\n\n");
    } else {
        printf(" [Statut : aucun labyrinthe charge]\n\n");
    }
    printf(" 1. Creer un labyrinthe\n");
    printf(" 2. Charger un labyrinthe\n");
    printf(" 3. Jouer\n");
    printf(" 4. Visualiser les 10 meilleurs scores\n");
    printf(" 5. Quitter\n");
    printf("========================================\n");
}

char ui_get_key(void) {
    struct termios oldt, newt;
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        int ch = getchar();
        return (ch == EOF) ? 0 : (char)ch;
    }

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return (ch == EOF) ? 0 : (char)ch;
}

int ui_prompt_menu_choice(void) {
    printf("Votre choix (1-5) : ");
    fflush(stdout);
    char c = ui_get_key();
    while (c == '\n' || c == '\r' || c == ' ') {
        c = ui_get_key();
    }
    if (c == 0) {
        printf("\n");
        return 5;
    }
    printf("%c\n", c);
    if (c >= '1' && c <= '5') {
        return c - '0';
    }
    return -1;
}

bool ui_prompt_string(const char *prompt, char *buffer, size_t max_len) {
    if (!prompt || !buffer || max_len == 0) {
        return false;
    }

    printf("%s", prompt);
    fflush(stdout);

    while (fgets(buffer, (int)max_len, stdin)) {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0) {
            return true;
        }
    }

    return false;
}

int ui_prompt_odd_int(const char *prompt, int min_val) {
    char buffer[32];
    while (1) {
        if (ui_prompt_string(prompt, buffer, sizeof(buffer))) {
            char *end = NULL;
            long val = strtol(buffer, &end, 10);
            if (end != buffer && *end == '\0') {
                if (val >= min_val && val % 2 != 0) {
                    return (int)val;
                }
            }
        } else if (feof(stdin)) {
            return (min_val % 2 != 0) ? min_val : min_val + 1;
        }
        printf("Valeur invalide. Veuillez entrer un nombre impair >= %d.\n", min_val);
    }
}

void ui_list_saved_mazes(const char *directory) {
    if (!directory) {
        return;
    }

    DIR *dir = opendir(directory);
    if (!dir) {
        printf("Aucun dossier '%s/' trouve pour l'instant.\n", directory);
        return;
    }

    printf("\nLabyrinthes disponibles dans '%s/' :\n", directory);
    int count = 0;
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        size_t name_len = strlen(entry->d_name);
        if (name_len > 4 && strcmp(entry->d_name + name_len - 4, ".cfg") == 0) {
            printf(" - %s\n", entry->d_name);
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf(" (Aucun fichier .cfg trouve)\n");
    }
    printf("\n");
}

void ui_display_message(const char *message) {
    if (message) {
        printf("\n%s\n", message);
    }
    printf("Appuyez sur une touche pour continuer...");
    fflush(stdout);
    ui_get_key();
    printf("\n");
}

void ui_display_hud(int score, int move_count, bool has_key) {
    printf("\n-------------------------------------------------------\n");
    printf(" Score : %-5d pts | Deplacements : %-4d | Cle : [%s]\n",
           score, move_count, has_key ? "OUI" : "NON");
    printf(" Symboles : K = Cle, $ = Bonus (+200), ! = Piege (-100)\n");
    printf("-------------------------------------------------------\n\n");
}

void ui_display_scoreboard(const ScoreBoard *board, const char *maze_name) {
    printf("\n=======================================================\n");
    printf("           TOP 10 DES MEILLEURS SCORES                 \n");
    if (maze_name && strlen(maze_name) > 0) {
        printf("           Labyrinthe : %s\n", maze_name);
    }
    printf("=======================================================\n");
    if (!board || board->count == 0) {
        printf("  Aucun score enregistre pour ce labyrinthe.\n");
    } else {
        printf("  RANG | %-20s | SCORE\n", "JOUEUR");
        printf("  -----+----------------------+----------\n");
        for (int i = 0; i < board->count; i++) {
            printf("   %2d. | %-20s | %6d pts\n",
                   i + 1, board->entries[i].name, board->entries[i].score);
        }
    }
    printf("=======================================================\n\n");
}
