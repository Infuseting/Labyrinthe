/**
 * @file monster.h
 * @brief Gestion des monstres et de leurs comportements via des pointeurs de fonction.
 * @author Serret Arthur
 */

#ifndef MONSTER_H
#define MONSTER_H

#include <stdbool.h>

/** @brief Nombre maximal de monstres dans un labyrinthe. */
#define MAX_MONSTERS 8

/** @brief Points de pénalité infligés par l'attaque d'un Ogre. */
#define OGRE_PENALTY 150

/** @brief Rayon d'étendue de mobilité de l'Ogre. */
#define OGRE_RADIUS 4

/** @brief Points de pénalité infligés par le contact d'un Fantôme. */
#define GHOST_PENALTY 75

/** @brief Rayon d'étendue de mobilité du Fantôme. */
#define GHOST_RADIUS 2

/**
 * @brief Énumération représentant le type d'un monstre.
 */
typedef enum {
    MONSTER_GHOST = 'F', /**< Fantôme : traverse les murs intérieurs */
    MONSTER_OGRE = 'O'   /**< Ogre : patrouille près des trésors sans traverser les murs */
} MonsterType;

/* Déclaration anticipée de la structure Maze */
struct Maze;

/* Déclaration anticipée de la structure Monster */
struct Monster;

/**
 * @brief Type du pointeur de fonction définissant la mobilité d'un monstre.
 *
 * @param[in,out] monster Pointeur vers le monstre à déplacer.
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
typedef void (*MonsterMoveFn)(struct Monster *monster, struct Maze *maze);

/**
 * @brief Structure représentant un monstre doté d'une règle de mobilité par pointeur de fonction.
 */
typedef struct Monster {
    MonsterType type;           /**< Type de monstre (Fantôme ou Ogre) */
    int x;                      /**< Position en colonne */
    int y;                      /**< Position en ligne */
    int origin_x;               /**< Colonne d'origine (centre du territoire de mobilité) */
    int origin_y;               /**< Ligne d'origine (centre du territoire de mobilité) */
    int territory_radius;       /**< Rayon maximal de l'étendue de mobilité */
    int penalty_points;         /**< Pénalité de score infligée en cas d'attaque */
    MonsterMoveFn move;         /**< Pointeur vers la fonction de mobilité */
} Monster;

/**
 * @brief Initialise un monstre avec ses paramètres selon son type et sa position de départ.
 *
 * @param[out] monster Pointeur vers le monstre à initialiser.
 * @param[in] type Type de monstre (MONSTER_GHOST ou MONSTER_OGRE).
 * @param[in] x Colonne initiale du monstre.
 * @param[in] y Ligne initiale du monstre.
 */
void monster_init(Monster *monster, MonsterType type, int x, int y);

/**
 * @brief Règle de mobilité du Fantôme : se déplace à travers les murs intérieurs dans son rayon.
 *
 * @param[in,out] monster Pointeur vers le fantôme.
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void monster_move_ghost(Monster *monster, struct Maze *maze);

/**
 * @brief Règle de mobilité de l'Ogre : reste dans les couloirs et patrouille près des trésors.
 *
 * @param[in,out] monster Pointeur vers l'ogre.
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void monster_move_ogre(Monster *monster, struct Maze *maze);

#endif
