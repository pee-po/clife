#include "clife_game.h"

/*
 * Declaration of internal functions
 */

/**
 * Generate a lookup table for determinig cell's next state
 *
 * The ownership is passed to the caller, needs to freed.
 * The table has following form: if the neighbourhood has state n then
 * n-th entry gives the midlle cells next state. State n is an unsigned integer
 * where LSB is the state of top-left neighbour of middle cell (alive == true);
 * next bit is the state of top neighbour, and so on (row by row).
 *
 * @param[in] life pointer to the life object.
 */
bool *generate_lookup_table(clife_t *life);

/*
 * Exported functions
 */

/* Memory */
clife_t *new_clife(uint32_t width, uint32_t height) {
    clife_t *life;
    bool *board, *board_sec;

    size_t cell_cnt = (size_t) width * (size_t) height;
    size_t board_size = sizeof *board * cell_cnt;

    /* Allocations */
    life = malloc(sizeof *life);
    board = malloc(board_size);
    board_sec = malloc(board_size);

    /* Check allocations */
    if (life == NULL || board == NULL || board_sec == NULL) {
        delete_clife(life);
        return NULL;
    }

    for (size_t i = 0; i < cell_cnt; i++) {
        board[i] = false;
        board_sec[i] = false;
    }

    /* Construct struct */
    life->board_ = board;
    life->board_next_ = board_sec;
    life->width = width;
    life->height = height;
    life->rule_b = 0;
    life->rule_s = 0;
    life->is_stable = false;
    life->lookup_table_ = NULL;
    
    return life;
} /* End new_clife */

void delete_clife(clife_t *life) {
    if (life != NULL) {
        free(life->board_);
        free(life->board_next_);
        free(life->lookup_table_);
    }
    free(life);
} /* End delete_clife */

bool clife_set_rule(clife_t *life, uint16_t rule_b, uint16_t rule_s) {
    life->rule_b = rule_b;
    life->rule_s = rule_s;
    bool *lookup_table = generate_lookup_table(life);
    if (lookup_table == NULL) {
        return false;
    } else {
        free(life->lookup_table_);
        life->lookup_table_ = lookup_table;
        return true;
    }
} /* End clife_set_rule */

/* Cells */
bool clife_get_cell(clife_t *life, uint32_t x, uint32_t y) {
    size_t offset = x + life->width * y;
    return *(life->board_ + offset);
} /* End clife_get_cell */

void clife_set_cell(clife_t *life, uint32_t x, uint32_t y, bool state) {
    size_t offset = x + life->width * y;
    *(life->board_ + offset) = state;
} /* End clife_set_cell */

/*
 * Internal functions
 */

bool *generate_lookup_table(clife_t *life) {
    uint16_t rule_b = life->rule_b;
    uint16_t rule_s = life->rule_s;
    uint16_t neighbourhood_size = 9; // 3x3;
    uint16_t combination_cnt = 512; // 2^neighbourhood_size;

    bool *table, *record;
    table = malloc(sizeof *table * combination_cnt);
    if (table == NULL) {
        return NULL;
    }

    uint16_t alive_cnt;
    for (uint16_t nstate = 0; nstate < combination_cnt; nstate ++) {
        bool mid_cell_alive = nstate & (1<<4);
        alive_cnt = 0;
        for (uint16_t cell = 0; cell < neighbourhood_size; cell++) {
            if (cell == 4) continue;
            if (nstate & (1<<cell)) alive_cnt++;
        }

        record = table + nstate;
        if (mid_cell_alive) {
            if (rule_s & (1<<alive_cnt)) {
                *record = true;
            } else {
                *record = false;
            }
        } else {
            if (rule_b & (1<<alive_cnt)) {
                *record = true;
            } else {
                *record = false;
            }
        }
    }
    return table;
} /* End generate_lookup_table */

