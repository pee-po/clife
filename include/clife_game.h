#ifndef _clife_game_h_include_
#define _clife_game_h_include_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * Type definitions
 */
typedef struct {
    bool *board_;
    bool *board_next_;
    uint32_t width;
    uint32_t height;
    uint16_t rule_b;
    uint16_t rule_s;
    bool is_stable;
    uint16_t *lookup_table_;
} clife_t;

typedef struct {
    bool state;
    uint32_t x;
    uint32_t y;
} clife_point_state;

typedef enum {OK, BUFF_SHORT, ERR} update_status;

/*
 * Function definitions
 */

/* Memory */

/**
 * Construct game of life's object with reasonable defaults.
 *
 * The object is allocated on the heap - the ownership is paseed to the
 * caller, it should be freed with delete_clife.
 *
 * @param[in] width grid's width given in number of cells
 * @param[in] width grid's hight given in number of cells
 * @param[out] life pointer to object;
 */
clife_t *new_clife(uint32_t width, uint32_t height);

/**
 * Destructs game of life's object and frees the memory.
 *
 * @param[in] life pointer to the object to be deallocated
 */
void delete_clife(clife_t *life);

/* General */
void clife_set_rule(clife_t *life, uint16_t rule_b, uint16_t rule_s);
void clife_step(clife_t *life);
update_status clife_step_get_updates(
    clife_t *life,
    clife_point_state *state_buff,
    uint64_t buff_len,
    uint64_t *update_len
);

/* Cells */
bool clife_get_cell(clife_t *life, uint32_t x, uint32_t y);
void clife_set_cell(clife_t *life, uint32_t x, uint32_t y, bool state);

#endif /* _clife_game_h_include_ */

