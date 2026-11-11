/**
 * @file main.c
 * @brief Point d'entrée principal du programme labyrinthe.
 * @author Serret Arthur
 * @date 2026-11-11
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"

/**
 * @brief Fonction principale générant et affichant un labyrinthe en mode console.
 *
 * Initialise le générateur aléatoire, crée un labyrinthe 11x25,
 * génère un chemin parfait selon l'annexe, puis l'affiche en console.
 *
 * @return EXIT_SUCCESS en cas de succès, EXIT_FAILURE en cas d'erreur.
 */
int main(void) {
    srand((unsigned int)time(NULL));

    Maze *maze = maze_create(DEFAULT_HEIGHT, DEFAULT_WIDTH);
    if (!maze) {
        return EXIT_FAILURE;
    }

    if (maze_generate(maze) != 0) {
        maze_free(maze);
        return EXIT_FAILURE;
    }

    maze_print(maze);
    maze_free(maze);

    return EXIT_SUCCESS;
}
