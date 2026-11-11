/**
 * @file score.c
 * @brief Gestion du tableau des 10 meilleurs scores.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"

/**
 * @brief Encode un nom de joueur pour la persistance : les '_' deviennent '__' et les espaces deviennent '_'.
 *
 * @param[in] src Nom d'origine.
 * @param[out] dst Buffer de destination pour le nom encodé sans espace.
 * @param[in] max_len Taille maximale du buffer de destination.
 */
static void score_encode_name(const char *src, char *dst, size_t max_len) {
    if (!src || !dst || max_len == 0) {
        return;
    }

    size_t out = 0;
    for (size_t in = 0; src[in] != '\0' && out + 1 < max_len; in++) {
        if (src[in] == '_') {
            if (out + 2 < max_len) {
                dst[out++] = '_';
                dst[out++] = '_';
            } else {
                break;
            }
        } else if (src[in] == ' ') {
            dst[out++] = '_';
        } else {
            dst[out++] = src[in];
        }
    }
    dst[out] = '\0';
}

/**
 * @brief Décode un nom de joueur lu dans un fichier : '__' redevient '_' et '_' isolé redevient un espace.
 *
 * @param[in] src Nom encodé lu depuis le fichier.
 * @param[out] dst Buffer de destination pour le nom restauré.
 * @param[in] max_len Taille maximale du buffer de destination.
 */
static void score_decode_name(const char *src, char *dst, size_t max_len) {
    if (!src || !dst || max_len == 0) {
        return;
    }

    size_t out = 0;
    for (size_t in = 0; src[in] != '\0' && out + 1 < max_len; in++) {
        if (src[in] == '_') {
            if (src[in + 1] == '_') {
                dst[out++] = '_';
                in++; /* Consomme le second '_' de l'échappement */
            } else {
                dst[out++] = ' ';
            }
        } else {
            dst[out++] = src[in];
        }
    }
    dst[out] = '\0';
}

void score_board_init(ScoreBoard *board) {
    if (!board) {
        return;
    }

    board->count = 0;
    for (int i = 0; i < MAX_SCORES; i++) {
        board->entries[i].name[0] = '\0';
        board->entries[i].score = 0;
    }
}

int score_board_load(ScoreBoard *board, const char *filepath) {
    if (!board || !filepath) {
        return -1;
    }

    score_board_init(board);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        return 0;
    }

    char encoded[128];
    int score = 0;

    while (board->count < MAX_SCORES && fscanf(file, "%127s %d", encoded, &score) == 2) {
        score_decode_name(encoded, board->entries[board->count].name, SCORE_NAME_MAX);
        board->entries[board->count].score = score;
        board->count++;
    }

    fclose(file);
    return 0;
}

int score_board_save(const ScoreBoard *board, const char *filepath) {
    if (!board || !filepath) {
        return -1;
    }

    FILE *file = fopen(filepath, "w");
    if (!file) {
        return -1;
    }

    for (int i = 0; i < board->count; i++) {
        char encoded[128];
        score_encode_name(board->entries[i].name, encoded, sizeof(encoded));
        fprintf(file, "%s %d\n", encoded, board->entries[i].score);
    }

    fclose(file);
    return 0;
}

bool score_board_is_top10(const ScoreBoard *board, int score) {
    if (!board) {
        return false;
    }

    if (board->count < MAX_SCORES) {
        return true;
    }

    return (score > board->entries[MAX_SCORES - 1].score);
}

bool score_board_add(ScoreBoard *board, const char *name, int score) {
    if (!board || !name || strlen(name) == 0) {
        return false;
    }

    int insert_idx = 0;
    while (insert_idx < board->count && board->entries[insert_idx].score >= score) {
        insert_idx++;
    }

    if (insert_idx >= MAX_SCORES) {
        return false;
    }

    int limit = (board->count < MAX_SCORES) ? board->count : (MAX_SCORES - 1);
    for (int i = limit; i > insert_idx; i--) {
        board->entries[i] = board->entries[i - 1];
    }

    snprintf(board->entries[insert_idx].name, SCORE_NAME_MAX, "%s", name);
    board->entries[insert_idx].score = score;

    if (board->count < MAX_SCORES) {
        board->count++;
    }

    return true;
}
