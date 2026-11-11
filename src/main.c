/**
 * @file main.c
 * @brief Point d'entrée principal du programme labyrinthe.
 * @author Serret Arthur
 */

#include <stdlib.h>
#include <time.h>
#include "game.h"

/**
 * @brief Fonction principale : initialise le générateur aléatoire et lance le jeu.
 *
 * @return EXIT_SUCCESS.
 */
int main(void) {
    srand((unsigned int)time(NULL));
    game_run();
    return EXIT_SUCCESS;
}
