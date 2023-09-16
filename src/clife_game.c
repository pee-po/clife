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

/**
 * Get neighbours of a given cell
 *
 * Returns neighbourhood state, where LSB is the state is top-left neighbour
 * of cell given by (col, row) (alize == true); next bit is the state of
 * top nwighbour, and so on (row by row).
 *
 * @param[in] life pointer to the life object.
 * @param[in] col horizontal position of the cell central to the
 * neighbourhood to be calculated.
 * @param[in] row vertical position of the cell central to the
 * neighbourhood to be calculated.
 */
uint16_t get_neighbours(clife_t *life, uint32_t col, uint32_t row);


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

void clife_step(clife_t *life) {
    uint16_t neighbourhood_state;
    size_t cell_offset;
    bool state;
    for (uint32_t row = 0; row < life->height; row++) {
        for (uint32_t col = 0; col < life->width; col++) {
            neighbourhood_state = get_neighbours(life, col, row);
            cell_offset = col + row*life->width;
            state = *(life->lookup_table_ + neighbourhood_state);
            *(life->board_next_ + cell_offset) = state;
        }
    }
    bool *temp_board;
    temp_board = life->board_;
    life->board_ = life->board_next_;
    life->board_next_ = temp_board;
} /* End clife_step */


/* Cells */
bool clife_get_cell(clife_t *life, uint32_t x, uint32_t y) {
    size_t offset = x + life->width * y;
    return *(life->board_ + offset);
} /* End clife_get_cell */

void clife_set_cell(clife_t *life, uint32_t x, uint32_t y, bool state) {
    size_t offset = x + life->width * y;
    *(life->board_ + offset) = state;
} /* End clife_set_cell */


/* Serialisation */
size_t clife_serialise(clife_t *life, char *buffer, size_t buff_len) {
    uint32_t max_col = life->width - 1;
    size_t copied_cnt = 0;
    bool state;

    char cycler = 7;
    char acc = 0;
    for (uint32_t row = 0; row < life->height; row++) {
        for (uint32_t col = 0; col <= max_col; col++) {
            if (copied_cnt >= buff_len) return 0;
            /*
             * Uses only an exported function to access cell state.
             * This is slow but decouples this function from board
             * implementation.
             */
            state = clife_get_cell(life, col, row);
            if (state) acc = acc | (1<<cycler);
            if (cycler == 0 || col == max_col) {
                *buffer = acc;
                buffer++;
                acc = 0;
                cycler = 7;
                copied_cnt++;
            } else {
                cycler--;
            }
        }
    }
    return copied_cnt;
} /* End clife_serialise */

size_t clife_deserialise(clife_t *life, char *buffer, size_t buff_len) {
    size_t bytes_processed = 0;
    uint32_t col = 0;
    uint32_t row = 0;
    bool state;

    while (buff_len) {
        for (char i = 7; i >= 0; i--) {
            /* ternary expresion is to keep bools clean */
            state = *buffer & (1<<i) ? true : false;
            /*
             * Uses only an exported function to access cell state.
             * This is slow but decouples this function from board
             * implementation.
             */
            clife_set_cell(life, col, row, state);
            col++;
            if (col > life->width) {
                col = 0;
                row++;
                if (row > life->height) {
                    return bytes_processed;
                }
                break;
            }
        }
        buffer++;
        bytes_processed++;
        buff_len--;
    }
    return bytes_processed;
} /* End clife_deserialise */


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

uint16_t get_neighbours(clife_t *life, uint32_t col, uint32_t row) {
    int16_t col_offset, row_offset;
    const uint32_t min_col = 0;
    const uint32_t max_col = life->width - 1;
    const uint32_t min_row = 0;
    const uint32_t max_row = life->height - 1;
    uint16_t neighbourhood = 0;
    size_t offset;

    for (uint16_t i = 0; i < 9; i++) {
        col_offset = i % 3 - 1;
        row_offset = i / 3 - 1;

        if (
            (col > min_col || col_offset >= 0)
            && (col < max_col || col_offset <= 0)
            && (row > min_row || row_offset >= 0)
            && (row < max_row || row_offset <= 0)
        ) {
            offset = col + col_offset + (row + row_offset)*life->width;
            if (*(life->board_ + offset)) {
                neighbourhood = neighbourhood | 1<<i;
            }
        }
    }
    return neighbourhood;
} /* End get_neighbours */

