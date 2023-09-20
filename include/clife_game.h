#ifndef _clife_game_h_include_
#define _clife_game_h_include_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 * Type definitions
 */
typedef struct clife_s clife_t;

typedef struct {
    bool state;
    uint32_t x;
    uint32_t y;
} clife_point_state;

typedef enum {OK, BUFF_SHORT, ERR} update_status;

/*
 * Macros
 */
#define clife_set_def_rule(x) clife_set_rule(x, 8, 12)

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
/**
 * Perform step calculations
 *
 * @param[in] life pointer to the life object
 */
void clife_step(clife_t *life);

/**
 * Perform step calculations and fill a buffer with cell status updates.
 *
 * As opposed to clife_step - it doesn't use the secondary board
 * (internal) representation. The cells that need to be updated are filled into
 * state_buff (which has to be pre-allocated). If the state_buff is filled
 * correctly, then the updates are applied to the internal board
 * representation. If the buffer is NULL or is shorter than the update
 * then BUFF_SHORT is returned and the updates are NOT applied to the board.
 * *update_len starts at 0. The state updates are placed into the buffer
 * starting at index = 0. Each time this is done, *update_len is incremented.
 * At the end *update_len is the number of cell state
 * updates reliably recorded into the buffer. The actual number of elements in
 * buffer that were modified may differ - caller should rely only on elements
 * of buffer indexed by a number smaller than *update_len.
 *
 * @param[in] life pointer to the life object
 * @param[in] state_buff status update buffer
 * (the ownership remains with the caller)
 * @param[in] buff_len buffer size (number of records)
 * @param[in] update_len pointer to a variable that will be set
 * to the number of records in buffer that have been modified.
 * (the ownership remains with the caller)
 * @param[out] update_status 
 */
update_status clife_step_get_updates(
    clife_t *life,
    clife_point_state *state_buff,
    uint64_t buff_len,
    uint64_t *update_len
);


/* Getters and setters */
/**
 * Get life object's width.
 *
 * @param[in] life pointer to the life object
 */
uint32_t clife_get_width(clife_t *life);

/**
 * Get life object's height.
 *
 * @param[in] life pointer to the life object
 */
uint32_t clife_get_height(clife_t *life);

/**
 * Get cell's state.
 *
 * @param[in] x cell's column
 * @param[in] y cell's row
 * @param[out] state cell's state
 */
bool clife_get_cell(clife_t *life, uint32_t x, uint32_t y);

/**
 * Set cell's state.
 *
 * @param[in] x cell's column
 * @param[in] y cell's row
 * @param[in] state cell's new state
 */
void clife_set_cell(clife_t *life, uint32_t x, uint32_t y, bool state);

/**
 * Set a basic rule for game of life.
 *
 * Set's a rule in form of BX/SY where X and Y are encoded to rule_b and rule_s
 * in following way: if rule R includes positive integer Z then Z-th bit of R
 * is set. E.g. the cannonical rule of B3/S23 is encoded as:
 * rule_b = 2^3 = 8; rule_s = 2^2 + 2^3 = 12.
 *
 * @param[in] life pointer to the life object
 * @param[in] rule_b unsigned integer representing "born" rule in such a way
 * that if n-th bit is set then n alive neighbours set the cell alive - if 
 * dead.
 * @param[in] rule_s unsigned integer representing "survive" rule in such a way
 * that if n-th bit is set then n alive neighbours keep the cell alive - if 
 * already alive.
 */
bool clife_set_rule(clife_t *life, uint16_t rule_b, uint16_t rule_s);


/* Serialisation */
/**
 * Serilise life object to a bytestream.
 *
 * Transcribes only cell states in a reversible way, that is - given other
 * members of life object are known, the resulting bytestream identifies
 * the board state uniquely. A reverse operation is clife_deserialise.
 *
 * @param[in] life pointer to the life object
 * @param[in] buffer an allocated char buffer. Ownership of the buffer remains
 * with the caller
 * @param[in] buff_len length of buffer
 * @param[out] size number of bytes written to the buffer. A value of 0
 * indicates that the size of buffer was insufficient.
 */
size_t clife_serialise(clife_t *life, char *buffer, size_t buff_len);

/**
 * Deserialise bytestream into life object.
 *
 * Sets cell states according to the information in bytestream.
 * A reverse operation is clife_serialise.
 *
 * @param[in] life pointer to the life object
 * @param[in] buffer an char buffer (read only)
 * @param[in] buff_len length of buffer
 * @param[out] bytes_processed number of bytes processed. Generally a
 * successful operation will have bytes_processed == buff_len. But this doesn't
 * safeguard against a situation where the buffer is to small compared to the
 * board and therfore not all cells are set.
 */
size_t clife_deserialise(clife_t *life, char *buffer, size_t buff_len);

#endif /* _clife_game_h_include_ */

