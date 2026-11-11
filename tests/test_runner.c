/**
 * @file test_runner.c
 * @brief Exécuteur principal agrégeant l'ensemble des suites de tests unitaires du projet.
 * @author Serret Arthur
 */

#include <stdio.h>

/**
 * @brief Exécute la suite de tests unitaires du module maze.
 *
 * @return 0 en cas de succès, nombre d'échecs sinon.
 */
int test_maze_run(void);

/**
 * @brief Exécute la suite de tests unitaires du module ui.
 *
 * @return 0 en cas de succès, nombre d'échecs sinon.
 */
int test_ui_run(void);

/**
 * @brief Exécute la suite de tests unitaires du module game.
 *
 * @return 0 en cas de succès, nombre d'échecs sinon.
 */
int test_game_run(void);

/**
 * @brief Exécute la suite de tests unitaires du module score.
 *
 * @return 0 en cas de succès, nombre d'échecs sinon.
 */
int test_score_run(void);

/**
 * @brief Exécute la suite de tests unitaires du module monster.
 *
 * @return 0 en cas de succès, nombre d'échecs sinon.
 */
int test_monster_run(void);

/**
 * @brief Point d'entrée de l'exécuteur de tests : lance tous les modules de test et rapporte le bilan.
 *
 * @return 0 si tous les tests de tous les modules ont réussi, 1 en cas d'au moins un échec.
 */
int main(void) {
    int total_failures = 0;

    printf("\n=======================================================\n");
    printf("         EXECUTION DE LA SUITE DE TESTS COMPLETE       \n");
    printf("=======================================================\n");

    printf("\n--- Module maze.h ---\n");
    total_failures += test_maze_run();

    printf("\n--- Module ui.h ---\n");
    total_failures += test_ui_run();

    printf("\n--- Module game.h ---\n");
    total_failures += test_game_run();

    printf("\n--- Module score.h ---\n");
    total_failures += test_score_run();

    printf("\n--- Module monster.h ---\n");
    total_failures += test_monster_run();

    printf("\n=======================================================\n");
    if (total_failures == 0) {
        printf(" SUCCES : Tous les modules de test ont reussi !\n");
    } else {
        printf(" ECHEC : %d assertion(s) ou test(s) echoue(s).\n", total_failures);
    }
    printf("=======================================================\n\n");

    return (total_failures > 0) ? 1 : 0;
}
