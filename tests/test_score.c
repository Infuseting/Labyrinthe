/**
 * @file test_score.c
 * @brief Tests unitaires du module de gestion des scores (score.h).
 * @author Serret Arthur
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"
#include "minunit.h"

/**
 * @brief Cas nominal : initialisation d'un tableau et insertion ordonnée de scores.
 */
MU_TEST(test_score_nominal_init_et_insertion) {
    ScoreBoard board;
    score_board_init(&board);
    mu_assert_int_eq(0, board.count);

    mu_check(score_board_is_top10(&board, 1500) == true);
    mu_check(score_board_add(&board, "Alice", 1500) == true);
    mu_assert_int_eq(1, board.count);
    mu_assert_string_eq("Alice", board.entries[0].name);
    mu_assert_int_eq(1500, board.entries[0].score);

    mu_check(score_board_add(&board, "Charlie", 1000) == true);
    mu_assert_int_eq(2, board.count);

    /* Insertion intermédiaire : Bob avec 1200 doit se placer en 2e position */
    mu_check(score_board_add(&board, "Bob", 1200) == true);
    mu_assert_int_eq(3, board.count);

    mu_assert_string_eq("Alice", board.entries[0].name);
    mu_assert_int_eq(1500, board.entries[0].score);
    mu_assert_string_eq("Bob", board.entries[1].name);
    mu_assert_int_eq(1200, board.entries[1].score);
    mu_assert_string_eq("Charlie", board.entries[2].name);
    mu_assert_int_eq(1000, board.entries[2].score);
}

/**
 * @brief Cas nominal : sauvegarde et rechargement d'un tableau de scores.
 */
MU_TEST(test_score_nominal_sauvegarde_et_chargement) {
    const char *test_file = "saves/test_score_nominal.score";

    ScoreBoard b1;
    score_board_init(&b1);
    score_board_add(&b1, "PlayerOne", 1800);
    score_board_add(&b1, "PlayerTwo", 1400);
    score_board_add(&b1, "PlayerThree", 950);

    mu_assert_int_eq(0, score_board_save(&b1, test_file));

    ScoreBoard b2;
    score_board_init(&b2);
    mu_assert_int_eq(0, score_board_load(&b2, test_file));

    mu_assert_int_eq(3, b2.count);
    mu_assert_string_eq("PlayerOne", b2.entries[0].name);
    mu_assert_int_eq(1800, b2.entries[0].score);
    mu_assert_string_eq("PlayerTwo", b2.entries[1].name);
    mu_assert_int_eq(1400, b2.entries[1].score);
    mu_assert_string_eq("PlayerThree", b2.entries[2].name);
    mu_assert_int_eq(950, b2.entries[2].score);

    remove(test_file);
}

/**
 * @brief Cas nominal : persistance fidèle des noms contenant des espaces, des underscores et des mélanges.
 */
MU_TEST(test_score_nominal_sauvegarde_avec_espaces_et_underscores) {
    const char *test_file = "saves/test_score_escape.score";

    ScoreBoard b1;
    score_board_init(&b1);
    score_board_add(&b1, "Jean Dupont", 1500);
    score_board_add(&b1, "Arthur_Pendragon", 1200);
    score_board_add(&b1, "Super_Gamer 2023", 1000);
    score_board_add(&b1, "Double__Under_ Space", 800);

    mu_assert_int_eq(0, score_board_save(&b1, test_file));

    ScoreBoard b2;
    score_board_init(&b2);
    mu_assert_int_eq(0, score_board_load(&b2, test_file));

    mu_assert_int_eq(4, b2.count);
    mu_assert_string_eq("Jean Dupont", b2.entries[0].name);
    mu_assert_int_eq(1500, b2.entries[0].score);

    mu_assert_string_eq("Arthur_Pendragon", b2.entries[1].name);
    mu_assert_int_eq(1200, b2.entries[1].score);

    mu_assert_string_eq("Super_Gamer 2023", b2.entries[2].name);
    mu_assert_int_eq(1000, b2.entries[2].score);

    mu_assert_string_eq("Double__Under_ Space", b2.entries[3].name);
    mu_assert_int_eq(800, b2.entries[3].score);

    remove(test_file);
}

/**
 * @brief Cas limite : vérification de l'éligibilité sur un tableau vide.
 */
MU_TEST(test_score_limite_tableau_vide) {
    ScoreBoard board;
    score_board_init(&board);

    mu_check(score_board_is_top10(&board, 0) == true);
    mu_check(score_board_is_top10(&board, -100) == true);
    mu_check(score_board_is_top10(&board, 5000) == true);
}

/**
 * @brief Cas limite : saturation à 10 scores et éviction du 10e score par un score supérieur.
 */
MU_TEST(test_score_limite_saturation_10_scores) {
    ScoreBoard board;
    score_board_init(&board);

    for (int i = 0; i < 10; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Joueur%d", i + 1);
        mu_check(score_board_add(&board, name, 1000 - i * 50) == true);
    }
    mu_assert_int_eq(10, board.count);
    mu_assert_int_eq(550, board.entries[9].score);

    /* Un score inférieur ou égal au 10e ne rentre pas */
    mu_check(score_board_is_top10(&board, 500) == false);
    mu_check(score_board_is_top10(&board, 550) == false);
    mu_check(score_board_add(&board, "Faible", 500) == false);
    mu_assert_int_eq(10, board.count);

    /* Un score supérieur au 10e rentre et évince le dernier */
    mu_check(score_board_is_top10(&board, 725) == true);
    mu_check(score_board_add(&board, "Nouveau", 725) == true);
    mu_assert_int_eq(10, board.count);

    /* Vérifier que le dernier score est maintenant 600 (550 a été évincé) */
    mu_assert_int_eq(600, board.entries[9].score);
}

/**
 * @brief Cas limite : insertion de scores identiques (égalité de points).
 */
MU_TEST(test_score_limite_egalite_scores) {
    ScoreBoard board;
    score_board_init(&board);

    mu_check(score_board_add(&board, "UserA", 1000) == true);
    mu_check(score_board_add(&board, "UserB", 1000) == true);
    mu_assert_int_eq(2, board.count);
    mu_assert_int_eq(1000, board.entries[0].score);
    mu_assert_int_eq(1000, board.entries[1].score);
}

/**
 * @brief Cas limite : troncature propre des noms dépassant la taille maximale.
 */
MU_TEST(test_score_limite_nom_long) {
    ScoreBoard board;
    score_board_init(&board);

    const char *long_name = "PseudoExtremementLongDepassantTrenteDeuxCaracteresComplets";
    mu_check(score_board_add(&board, long_name, 1200) == true);
    mu_assert_int_eq(1, board.count);
    mu_check(strlen(board.entries[0].name) < SCORE_NAME_MAX);
}

/**
 * @brief Cas aléatoire : insertion de 50 scores aléatoires et vérification de l'ordre décroissant strict.
 */
MU_TEST(test_score_aleatoire_tri_multiples_scores) {
    ScoreBoard board;
    score_board_init(&board);

    for (int i = 0; i < 50; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Bot%d", i);
        int rand_score = rand() % 3000;
        score_board_add(&board, name, rand_score);
    }

    mu_assert_int_eq(MAX_SCORES, board.count);

    for (int i = 0; i < board.count - 1; i++) {
        mu_check(board.entries[i].score >= board.entries[i + 1].score);
    }
}

/**
 * @brief Cas aléatoire : cycle de sauvegarde et rechargement de scores aléatoires.
 */
MU_TEST(test_score_aleatoire_sauvegarde_fuzz) {
    const char *fuzz_file = "saves/test_score_fuzz.score";

    ScoreBoard b1;
    score_board_init(&b1);
    int count = 1 + rand() % MAX_SCORES;
    for (int i = 0; i < count; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Fuzz%d", i);
        score_board_add(&b1, name, (rand() % 2000) + 100);
    }

    mu_assert_int_eq(0, score_board_save(&b1, fuzz_file));

    ScoreBoard b2;
    score_board_init(&b2);
    mu_assert_int_eq(0, score_board_load(&b2, fuzz_file));

    mu_assert_int_eq(b1.count, b2.count);
    for (int i = 0; i < b1.count; i++) {
        mu_assert_string_eq(b1.entries[i].name, b2.entries[i].name);
        mu_assert_int_eq(b1.entries[i].score, b2.entries[i].score);
    }

    remove(fuzz_file);
}

/**
 * @brief Cas d'erreur : vérification de la résilience face à des pointeurs NULL ou noms vides.
 */
MU_TEST(test_score_erreur_pointeurs_null) {
    ScoreBoard board;
    score_board_init(&board);

    score_board_init(NULL);
    mu_assert_int_eq(-1, score_board_load(NULL, "dummy.score"));
    mu_assert_int_eq(-1, score_board_load(&board, NULL));
    mu_assert_int_eq(-1, score_board_save(NULL, "dummy.score"));
    mu_assert_int_eq(-1, score_board_save(&board, NULL));
    mu_check(score_board_is_top10(NULL, 100) == false);
    mu_check(score_board_add(NULL, "Alice", 100) == false);
    mu_check(score_board_add(&board, NULL, 100) == false);
    mu_check(score_board_add(&board, "", 100) == false);
}

/**
 * @brief Cas d'erreur : chargement d'un fichier inexistant ou sauvegarde dans un chemin invalide.
 */
MU_TEST(test_score_erreur_fichier_invalide) {
    ScoreBoard board;
    score_board_init(&board);

    /* Fichier inexistant : doit réinitialiser le board et retourner 0 sans crasher */
    mu_assert_int_eq(0, score_board_load(&board, "saves/fichier_absolument_inexistant_9999.score"));
    mu_assert_int_eq(0, board.count);

    /* Chemin invalide d'écriture */
    mu_assert_int_eq(-1, score_board_save(&board, "/dossier_invalide_9999/test.score"));
}

/**
 * @brief Suite de tests pour le module score.
 */
MU_TEST_SUITE(score_test_suite) {
    MU_RUN_TEST(test_score_nominal_init_et_insertion);
    MU_RUN_TEST(test_score_nominal_sauvegarde_et_chargement);
    MU_RUN_TEST(test_score_nominal_sauvegarde_avec_espaces_et_underscores);
    MU_RUN_TEST(test_score_limite_tableau_vide);
    MU_RUN_TEST(test_score_limite_saturation_10_scores);
    MU_RUN_TEST(test_score_limite_egalite_scores);
    MU_RUN_TEST(test_score_limite_nom_long);
    MU_RUN_TEST(test_score_aleatoire_tri_multiples_scores);
    MU_RUN_TEST(test_score_aleatoire_sauvegarde_fuzz);
    MU_RUN_TEST(test_score_erreur_pointeurs_null);
    MU_RUN_TEST(test_score_erreur_fichier_invalide);
}

/**
 * @brief Exécute la suite de tests unitaires du module score.
 *
 * @return 0 si tous les tests ont réussi, nombre d'échecs sinon.
 */
int test_score_run(void) {
    MU_RUN_SUITE(score_test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
