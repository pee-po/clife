#include "clife_game.h"

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

