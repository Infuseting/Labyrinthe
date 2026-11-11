/**
 * @file maze.h
 * @brief Gestion et génération du labyrinthe.
 * @author Serret Arthur
 */

#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include "monster.h"

/** @brief Hauteur par défaut du labyrinthe. */
#define DEFAULT_HEIGHT 11

/** @brief Largeur par défaut du labyrinthe. */
#define DEFAULT_WIDTH 25

/** @brief Nombre maximal de positions mémorisées pour la traînée du joueur. */
#define MAX_TRAIL 10

/** @brief Nombre de points attribués par trésor ramassé. */
#define BONUS_POINTS 200

/** @brief Nombre de points retirés par piège déclenché. */
#define MALUS_POINTS 100

/** @brief Coût en points par déplacement effectué. */
#define MOVE_COST 1

/** @brief Score de départ accordé au joueur en début de partie. */
#define BASE_SCORE 1000

/** @brief Nombre de trésors générés par défaut. */
#define NUM_BONUSES 3

/** @brief Nombre de pièges générés par défaut. */
#define NUM_MALUSES 3

/**
 * @brief Énumération des niveaux de difficulté du labyrinthe.
 */
typedef enum {
    DIFF_EASY = 1, /**< Labyrinthe parfait avec bonus/malus sans monstre */
    DIFF_HARD = 2  /**< Labyrinthe avec boucles (chemins multiples) et monstres */
} Difficulty;

/**
 * @brief Énumération représentant le type d'une cellule du labyrinthe.
 */
typedef enum {
    WALL = '#',  /* Répresente un mur plein */
    EMPTY = ' ', /* Répresente un espace vide */
    KEY = 'K',   /* Clé permettant d'ouvrir la porte de sortie */
    BONUS = '$', /* Trésor accordant un bonus de points */
    MALUS = '!'  /* Piège infligeant un malus de points */
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
typedef struct Maze {
    int height;                     /* Hauteur de la grille */
    int width;                      /* Largeur de la grille */
    Difficulty difficulty;          /* Niveau de difficulté (Facile ou Difficile) */
    Coords player;                  /* Position du joueur */
    Coords exit;                    /* Position de la sortie */
    Coords trail[MAX_TRAIL];        /* Historique des 10 dernières positions du joueur */
    int trail_count;                /* Nombre actuel de positions dans la traînée */
    bool has_key;                   /* Indique si le joueur possède la clé */
    int score;                      /* Score actuel de la partie */
    int move_count;                 /* Nombre de déplacements effectués */
    Monster monsters[MAX_MONSTERS]; /* Monstres actifs dans le labyrinthe */
    int monster_count;              /* Nombre actuel de monstres actifs */
    char last_alert[128];           /* Message du dernier événement (ex: attaque de monstre) */
    Cell **grid;                    /* Matrice 2D des cellules */
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
 * @brief Génère le labyrinthe selon son niveau de difficulté actuel.
 *
 * @param[out] maze Pointeur vers le labyrinthe à générer.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int maze_generate(Maze *maze);

/**
 * @brief Génère le labyrinthe en appliquant explicitement le niveau de difficulté spécifié.
 *
 * @param[out] maze Pointeur vers le labyrinthe à générer.
 * @param[in] difficulty Difficulté souhaitée (DIFF_EASY ou DIFF_HARD).
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int maze_generate_with_difficulty(Maze *maze, Difficulty difficulty);

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
 * @return true si le déplacement a été effectué, false si bloqué par un mur, sortie verrouillée ou hors limites.
 */
bool maze_move(Maze *maze, int dx, int dy);

/**
 * @brief Vérifie si le joueur a atteint la porte de sortie.
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @return true si le joueur est sur la case de sortie, false sinon.
 */
bool maze_is_exit_reached(const Maze *maze);

/**
 * @brief Réinitialise la traînée du joueur.
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void maze_reset_trail(Maze *maze);

/**
 * @brief Répartit aléatoirement la clé, les trésors et les pièges sur les cases vides.
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 * @param[in] num_bonuses Nombre de trésors à placer.
 * @param[in] num_maluses Nombre de pièges à placer.
 */
void maze_spawn_objects(Maze *maze, int num_bonuses, int num_maluses);

/**
 * @brief Fait apparaître les monstres (fantômes et ogres) sur des cases libres en mode difficile.
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void maze_spawn_monsters(Maze *maze);

/**
 * @brief Déplace tous les monstres du labyrinthe via leurs pointeurs de fonctions respectifs.
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void maze_update_monsters(Maze *maze);

/**
 * @brief Recherche si un monstre est présent aux coordonnées spécifiées.
 *
 * @param[in] maze Pointeur vers le labyrinthe.
 * @param[in] x Colonne recherchée.
 * @param[in] y Ligne recherchée.
 * @return Pointeur constant vers le monstre trouvé, ou NULL si aucun.
 */
const Monster *maze_get_monster_at(const Maze *maze, int x, int y);

/**
 * @brief Réinitialise l'état de jeu du labyrinthe avant une nouvelle partie.
 *
 * @param[in,out] maze Pointeur vers le labyrinthe.
 */
void maze_reset_game_state(Maze *maze);

#endif
