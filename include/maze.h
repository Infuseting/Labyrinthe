/**
 * @file maze.h
 * @brief Gestion et génération du labyrinthe.
 * @author Serret Arthur
 */

#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

/** @brief Hauteur par défaut du labyrinthe. */
#define DEFAULT_HEIGHT 11

/** @brief Largeur par défaut du labyrinthe. */
#define DEFAULT_WIDTH 25

/**
 * @brief Énumération représentant le type d'une cellule du labyrinthe.
 */
typedef enum {
    WALL = '#', /* Répresente un mur plein */
    EMPTY = ' ' /* Répresente un espace vide */
} Cell;

/**
 * @brief Coordonnées 2D dans la grille du labyrinthe.
 */
typedef struct {
    int x; /* Coordonnées en colonne */
    int y; /* Coordonnées en ligne */
} Coords;

/**
 * @brief Structure représentant un labyrinthe complet.
 */
typedef struct {
    int height;    /* Hauteur de la grille */
    int width;     /* Largeur de la grille */
    Coords player; /* Position du joueur */
    Coords exit;   /* Position de la sortie */
    Cell **grid;   /* Matrice 2D des cellules */
} Maze;

/**
 * @brief Alloue et initialise un labyrinthe aux dimensions spécifiées.
 *
 * @param[in] height Hauteur du labyrinthe (doit être impaire et >= 3).
 * @param[in] width Largeur du labyrinthe (doit être impaire et >= 3).
 * @return Pointeur vers le labyrinthe alloué, ou NULL en cas d'erreur.
 */
Maze *maze_create(int height, int width);

/**
 * @brief Libère la mémoire allouée pour le labyrinthe et sa grille.
 *
 * @param[in] maze Pointeur vers le labyrinthe à libérer (peut être NULL).
 */
void maze_free(Maze *maze);

/**
 * @brief Génère un labyrinthe parfait avec l'algorithme de fusion aléatoire de chemins.
 *
 * @param[out] maze Pointeur vers le labyrinthe à générer.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int maze_generate(Maze *maze);

/**
 * @brief Affiche le labyrinthe sur la sortie standard.
 *
 * @param[in] maze Pointeur vers le labyrinthe à afficher.
 */
void maze_print(const Maze *maze);

/**
 * @brief Sauvegarde le labyrinthe dans un fichier au format .cfg.
 *
 * @param[in] maze Pointeur vers le labyrinthe à sauvegarder.
 * @param[in] filepath Chemin du fichier de destination.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int maze_save(const Maze *maze, const char *filepath);

/**
 * @brief Charge un labyrinthe depuis un fichier au format .cfg.
 *
 * @param[in] filepath Chemin du fichier source.
 * @return Pointeur vers le labyrinthe chargé, ou NULL en cas d'erreur.
 */
Maze *maze_load(const char *filepath);

/**
 * @brief Tente de déplacer le joueur selon un vecteur de déplacement (dx, dy).
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 * @param[in] dx Déplacement horizontal (-1, 0, 1).
 * @param[in] dy Déplacement vertical (-1, 0, 1).
 * @return true si le déplacement a été effectué, false si bloqué par un mur ou hors limites.
 */
bool maze_move(Maze *maze, int dx, int dy);

/**
 * @brief Vérifie si le joueur a atteint la porte de sortie.
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @return true si le joueur est sur la case de sortie, false sinon.
 */
bool maze_is_exit_reached(const Maze *maze);

#endif
