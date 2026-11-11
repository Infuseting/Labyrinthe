/**
 * @file score.c
 * @brief Gestion du tableau des 10 meilleurs scores.
 * @author Serret Arthur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"

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

    char name[SCORE_NAME_MAX];
    int score = 0;

    while (board->count < MAX_SCORES && fscanf(file, "%31s %d", name, &score) == 2) {
        snprintf(board->entries[board->count].name, SCORE_NAME_MAX, "%s", name);
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
        fprintf(file, "%s %d\n", board->entries[i].name, board->entries[i].score);
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
