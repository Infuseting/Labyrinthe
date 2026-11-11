/**
 * @file score.h
 * @brief Gestion du tableau des 10 meilleurs scores (Top 10) et des fichiers .score.
 * @author Serret Arthur
 */

#ifndef SCORE_H
#define SCORE_H

#include <stdbool.h>
#include <stddef.h>

/** @brief Nombre maximal d'entrées dans le tableau des meilleurs scores. */
#define MAX_SCORES 10

/** @brief Taille maximale du nom d'un joueur dans le score. */
#define SCORE_NAME_MAX 32

/**
 * @brief Entrée individuelle dans le tableau des scores.
 */
typedef struct {
    char name[SCORE_NAME_MAX]; /**< Nom du joueur */
    int score;                 /**< Score obtenu */
} ScoreEntry;

/**
 * @brief Tableau contenant les meilleurs scores d'un labyrinthe.
 */
typedef struct {
    ScoreEntry entries[MAX_SCORES]; /**< Liste des entrées de score */
    int count;                      /**< Nombre actuel d'entrées (max 10) */
} ScoreBoard;

/**
 * @brief Initialise un tableau de scores vide.
 *
 * @param[out] board Pointeur vers le tableau de scores à initialiser.
 */
void score_board_init(ScoreBoard *board);

/**
 * @brief Charge les scores depuis un fichier au format .score.
 *
 * @param[out] board Pointeur vers le tableau recevant les scores.
 * @param[in] filepath Chemin vers le fichier .score.
 * @return 0 en cas de succès, -1 en cas d'erreur de lecture.
 */
int score_board_load(ScoreBoard *board, const char *filepath);

/**
 * @brief Enregistre les scores dans un fichier au format .score.
 *
 * @param[in] board Pointeur vers le tableau de scores à sauvegarder.
 * @param[in] filepath Chemin vers le fichier .score.
 * @return 0 en cas de succès, -1 en cas d'erreur d'écriture.
 */
int score_board_save(const ScoreBoard *board, const char *filepath);

/**
 * @brief Vérifie si un score donné peut entrer dans le Top 10.
 *
 * @param[in] board Pointeur vers le tableau de scores.
 * @param[in] score Valeur du score à tester.
 * @return true si le score est éligible au Top 10, false sinon.
 */
bool score_board_is_top10(const ScoreBoard *board, int score);

/**
 * @brief Insère un nouveau score dans le tableau en maintenant l'ordre décroissant.
 *
 * @param[in,out] board Pointeur vers le tableau de scores.
 * @param[in] name Nom du joueur.
 * @param[in] score Score obtenu.
 * @return true si le score a été inséré dans le Top 10, false sinon.
 */
bool score_board_add(ScoreBoard *board, const char *name, int score);

#endif
