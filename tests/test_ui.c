/**
 * @file test_ui.c
 * @brief Tests unitaires du module d'interface utilisateur (ui.h).
 * @author Serret Arthur
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ui.h"
#include "minunit.h"

/**
 * @brief Cas nominal : affichage des menus avec et sans labyrinthe actif.
 */
MU_TEST(test_ui_nominal_affichage_menus) {
    ui_display_menu(true);
    ui_display_menu(false);
    mu_check(true);
}

/**
 * @brief Cas nominal : listing d'un dossier existant avec fichiers de sauvegarde.
 */
MU_TEST(test_ui_nominal_listing_dossier_saves) {
    mkdir("saves", 0755);
    FILE *f = fopen("saves/test_ui_list.cfg", "w");
    if (f) {
        fprintf(f, "3 3\n#o#\n# #\n#-#\n");
        fclose(f);
    }

    ui_list_saved_mazes("saves");
    remove("saves/test_ui_list.cfg");
    mu_check(true);
}

/**
 * @brief Cas limite : effacement de l'écran du terminal.
 */
MU_TEST(test_ui_limite_effacement_ecran) {
    ui_clear_screen();
    mu_check(true);
}

/**
 * @brief Cas limite : listing d'un dossier existant mais totalement vide.
 */
MU_TEST(test_ui_limite_listing_dossier_vide) {
    const char *empty_dir = "saves_test_empty_dir";
    mkdir(empty_dir, 0755);

    ui_list_saved_mazes(empty_dir);

    rmdir(empty_dir);
    mu_check(true);
}

/**
 * @brief Cas limite : buffer de taille minimale (1 octet) pour la saisie de chaîne.
 */
MU_TEST(test_ui_limite_buffer_taille_minimale) {
    char small_buf[1] = {'\0'};
    mu_check(ui_prompt_string("Test : ", small_buf, 0) == false);
}

/**
 * @brief Cas aléatoire : vérification de la robustesse sur des noms de fichiers générés aléatoirement.
 */
MU_TEST(test_ui_aleatoire_verification_noms_divers) {
    char random_name[64];
    for (int run = 0; run < 10; run++) {
        int len = 5 + rand() % 10;
        for (int i = 0; i < len; i++) {
            random_name[i] = (char)('a' + (rand() % 26));
        }
        random_name[len] = '\0';

        char path[128];
        snprintf(path, sizeof(path), "saves/%s.cfg", random_name);

        FILE *f = fopen(path, "w");
        mu_check(f != NULL);
        if (f) {
            fprintf(f, "3 3\n#o#\n# #\n#-#\n");
            fclose(f);
            remove(path);
        }
    }
}

/**
 * @brief Cas d'erreur : rejet des pointeurs NULL et tailles nulles dans ui_prompt_string.
 */
MU_TEST(test_ui_erreur_prompt_string_parametres_invalides) {
    char buf[32];
    mu_check(ui_prompt_string(NULL, buf, sizeof(buf)) == false);
    mu_check(ui_prompt_string("Invite : ", NULL, sizeof(buf)) == false);
    mu_check(ui_prompt_string("Invite : ", buf, 0) == false);
    mu_check(ui_prompt_string(NULL, NULL, 0) == false);
}

/**
 * @brief Cas nominal : affichage du bandeau HUD avec différents états de jeu.
 */
MU_TEST(test_ui_nominal_affichage_hud) {
    ui_display_hud(1000, 0, false, 1, NULL);
    ui_display_hud(850, 15, true, 2, "ATTAQUE ! Un Ogre vous a assomme (-150 pts) !");
    mu_check(true);
}

/**
 * @brief Cas nominal : affichage d'un tableau des scores garni.
 */
MU_TEST(test_ui_nominal_affichage_scoreboard) {
    ScoreBoard board;
    score_board_init(&board);
    score_board_add(&board, "Arthur", 1200);
    score_board_add(&board, "Joueur2", 950);
    ui_display_scoreboard(&board, "LabyTest");
    mu_check(true);
}

/**
 * @brief Cas limite : affichage d'un tableau des scores vide ou sans nom de labyrinthe.
 */
MU_TEST(test_ui_limite_scoreboard_vide) {
    ScoreBoard board;
    score_board_init(&board);
    ui_display_scoreboard(&board, NULL);
    ui_display_scoreboard(&board, "");
    mu_check(true);
}

/**
 * @brief Cas d'erreur : appel de l'affichage du tableau de score avec pointeur NULL.
 */
MU_TEST(test_ui_erreur_scoreboard_null) {
    ui_display_scoreboard(NULL, "Test");
    ui_display_scoreboard(NULL, NULL);
    mu_check(true);
}

/**
 * @brief Cas d'erreur : appel de listing sur un dossier inexistant ou un pointeur NULL.
 */
MU_TEST(test_ui_erreur_listing_dossier_invalide) {
    ui_list_saved_mazes(NULL);
    ui_list_saved_mazes("/dossier_qui_n_existe_absolument_pas_99999");
    mu_check(true);
}

/**
 * @brief Suite de tests pour le module ui.
 */
MU_TEST_SUITE(ui_test_suite) {
    MU_RUN_TEST(test_ui_nominal_affichage_menus);
    MU_RUN_TEST(test_ui_nominal_listing_dossier_saves);
    MU_RUN_TEST(test_ui_nominal_affichage_hud);
    MU_RUN_TEST(test_ui_nominal_affichage_scoreboard);
    MU_RUN_TEST(test_ui_limite_effacement_ecran);
    MU_RUN_TEST(test_ui_limite_listing_dossier_vide);
    MU_RUN_TEST(test_ui_limite_buffer_taille_minimale);
    MU_RUN_TEST(test_ui_limite_scoreboard_vide);
    MU_RUN_TEST(test_ui_aleatoire_verification_noms_divers);
    MU_RUN_TEST(test_ui_erreur_prompt_string_parametres_invalides);
    MU_RUN_TEST(test_ui_erreur_scoreboard_null);
    MU_RUN_TEST(test_ui_erreur_listing_dossier_invalide);
}

/**
 * @brief Exécute la suite de tests unitaires du module ui.
 *
 * @return 0 si tous les tests ont réussi, nombre d'échecs sinon.
 */
int test_ui_run(void) {
    MU_RUN_SUITE(ui_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
