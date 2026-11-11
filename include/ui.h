/**
 * @file ui.h
 * @brief Fonctions d'interaction utilisateur et d'affichage console.
 * @author Serret Arthur
 */

#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stddef.h>
#include "score.h"

/**
 * @brief Efface l'écran du terminal.
 */
void ui_clear_screen(void);

/**
 * @brief Affiche le menu principal du jeu.
 *
 * @param[in] has_active_maze Indique si un labyrinthe est actuellement chargé.
 */
void ui_display_menu(bool has_active_maze);

/**
 * @brief Lit un caractère immédiatement au clavier sans attendre l'appui sur Entrée.
 *
 * @return Caractère lu au clavier.
 */
char ui_get_key(void);

/**
 * @brief Demande à l'utilisateur de choisir une option dans le menu (1 à 5).
 *
 * @return Entier correspondant au choix sélectionné.
 */
int ui_prompt_menu_choice(void);

/**
 * @brief Demande la saisie sécurisée d'une chaîne de caractères.
 *
 * @param[in] prompt Message d'invite affiché à l'utilisateur.
 * @param[out] buffer Buffer mémoire pour stocker la chaîne saisie.
 * @param[in] max_len Taille maximale du buffer.
 * @return true si la saisie est non-vide, false sinon.
 */
bool ui_prompt_string(const char *prompt, char *buffer, size_t max_len);

/**
 * @brief Demande la saisie d'un entier impair supérieur ou égal à une valeur minimale.
 *
 * @param[in] prompt Message d'invite affiché à l'utilisateur.
 * @param[in] min_val Valeur minimale acceptée.
 * @return Entier impair valide saisi.
 */
int ui_prompt_odd_int(const char *prompt, int min_val);

/**
 * @brief Affiche la liste des fichiers de sauvegarde existants dans un répertoire.
 *
 * @param[in] directory Répertoire contenant les fichiers de sauvegarde.
 */
void ui_list_saved_mazes(const char *directory);

/**
 * @brief Demande à l'utilisateur de choisir un niveau de difficulté (1: Facile, 2: Difficile).
 *
 * @return 1 pour Facile, 2 pour Difficile.
 */
int ui_prompt_difficulty(void);

/**
 * @brief Affiche un message d'information ou d'erreur et attend un appui sur une touche.
 *
 * @param[in] message Message à afficher.
 */
void ui_display_message(const char *message);

/**
 * @brief Affiche le bandeau d'état en cours de partie (Score, Déplacements, Clé, Difficulté, Alertes).
 *
 * @param[in] score Score actuel du joueur.
 * @param[in] move_count Nombre de déplacements effectués.
 * @param[in] has_key Indique si le joueur a ramassé la clé.
 * @param[in] difficulty Niveau de difficulté (1: Facile, 2: Difficile).
 * @param[in] alert Message d'alerte récent (ex: attaque de monstre), ou NULL si aucun.
 */
void ui_display_hud(int score, int move_count, bool has_key, int difficulty, const char *alert);

/**
 * @brief Affiche le tableau des 10 meilleurs scores d'un labyrinthe.
 *
 * @param[in] board Pointeur vers le tableau des scores.
 * @param[in] maze_name Nom du labyrinthe concerné.
 */
void ui_display_scoreboard(const ScoreBoard *board, const char *maze_name);

#endif
