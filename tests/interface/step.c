#include "clife_game.h"
#include "test_utils.c"
#include <string.h>

/*
 * Type definitions
 */

 /**
  * Structure representing parsed life step args
  * 
  * Structure representing parsed life step args, for list of arguments,
  * see parse_args.
  */
struct parsed_args {
    uint8_t *bytes_1;
    uint8_t *bytes_2;
    size_t num_bytes_1;
    size_t num_bytes_2;
    uint32_t width;
    uint32_t height;
    uint16_t rule_b;
    uint16_t rule_s;
};

/*
 * Function prototypes
 */

/**
 * Test life step.
 *
 * Takes step args (see parsed_args). Allocates and initialises life object
 * according to bytes_1, width, height, rule_b, rule_s. Then performs a step
 * and compares deserialised life object to bytes_2. Step is performed twice,
 * separately: clife_step and clife_step_get_updates. In the latter case,
 * updates are also tested.
 */
int main(int argc, char **argv);

/**
 * Parse step args
 * 
 * Parse CLI args according to the specification:
 * * Hexstring to deserialise into a life object;
 * * Number of cells in width to allocate;
 * * Number of cells in height to allocate;
 * * Hexstring for comparison after the step;
 * * Rule_b (optional, default: 8) see clife_set_rule;
 * * Rule_s (optional, default: 12) see clife_set_rule;
 */
struct parsed_args *parse_args(int argc, char **argv);

/**
 * Free parsed_args
 * 
 * @param args pointer to parsed_args to free;
 */
void free_args(struct parsed_args *args);

/**
 * Test array of clife_point_state against life pre and post step.
 * 
 * @param[in] life_base Basline life object
 * @param[in] life Life object equivalent to life_base after a step 
 * @param[in] updates pointer to array of updates, generated while obtaining
 * life argument via clife_step_get_updates.
 * @param[in] update_len length of updates, generated while obtaining life
 * argument via clife_step_get_updates.
 * @param[out] int 0 if OK
 */
int validate_updates(
    clife_t *life_base, clife_t* life,
    struct clife_point_state *updates, uint64_t update_len
);

/**
 * Find point in updates array
 *
 * Find such i < update_len, that (x, y) == (updates[i].x, updates[i].y). If
 * found, return updates + i, else return NULL.
 */
struct clife_point_state *find_point_in_updates(
    uint32_t x, uint32_t y,
    struct clife_point_state *updates, uint64_t update_len
);

/*
 * Function definitions
 */
int main(int argc, char **argv) {
    struct parsed_args *args = parse_args(argc, argv);
    if (args->num_bytes_1 != args->num_bytes_2) {
        free_args(args);
        return 1;
    }
    size_t num_bytes = args->num_bytes_1;

    /* Set up life and buffer */
    uint32_t w, h;
    w = args->width;
    h = args->height;
    clife_t *life = new_clife(w, h);
    clife_t *life_cmp = new_clife(w, h);
    struct clife_point_state *updates;
    updates = malloc(sizeof *updates * w * h);
    uint8_t *buff;
    buff = malloc(sizeof *buff * num_bytes);
    if (
        buff == NULL
        || life == NULL
        || life_cmp == NULL
        || updates == NULL
        || !clife_set_rule(life, args->rule_b, args->rule_s)
    ) {
        free_args(args);
        delete_clife(life);
        free(buff);
        return 2;
    }

    /* Set cells, do step, compare */
    clife_deserialise(life, args->bytes_1, num_bytes);
    clife_step(life);
    clife_serialise(life, buff, num_bytes);
    int cmp_res_1 = memcmp(buff, args->bytes_2, num_bytes);

    /* Other step fun */
    clife_deserialise(life, args->bytes_1, num_bytes);
    clife_deserialise(life_cmp, args->bytes_1, num_bytes);
    uint64_t update_len;
    clife_step_get_updates(life, updates, w * h, &update_len);
    clife_serialise(life, buff, num_bytes);
    int cmp_res_2 = memcmp(buff, args->bytes_2, num_bytes);
    if (validate_updates(life_cmp, life, updates, update_len))
        return 1;
    
    /* Memory */
    free(buff);
    delete_clife(life);
    free_args(args);
    if (cmp_res_1 != cmp_res_2) return 1;
    return cmp_res_1;
} /* End main */

struct parsed_args *parse_args(int argc, char **argv) {
    struct parsed_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(1);

    args->bytes_1 = parse_hexstr(argv[1], &(args->num_bytes_1), NULL);
    args->bytes_2 = parse_hexstr(argv[4], &(args->num_bytes_2), NULL);

    if (args->bytes_1 == NULL || args->bytes_1 == NULL) {
        free_args(args);
        exit(2);
    }

    args->width = strtoul(argv[2], NULL, 10);
    args->height = strtoul(argv[3], NULL, 10);
    if (argc > 5) args->rule_b = strtoul(argv[5], NULL, 10);
    else args->rule_b = 8;
    if (argc > 6) args->rule_s = strtoul(argv[6], NULL, 10);
    else args->rule_s = 12;

    if (errno) {
        free_args(args);
        exit(3);
    }

    return args;
} /* End parse_args */

void free_args(struct parsed_args *args) {
    if (args != NULL) {
        free(args->bytes_1);
        free(args->bytes_2);
        free(args);
    }
} /* End free_args */

int validate_updates(
    clife_t *life_base, clife_t* life,
    struct clife_point_state *updates, uint64_t update_len
) {
    bool cstate, cstate_cmp, expected_in_update;
    struct clife_point_state *pt_state;
    for (uint32_t col = 0; col < clife_get_width(life); col++) {
        for (uint32_t row = 0; row < clife_get_height(life); row++) {
            cstate_cmp = clife_get_cell(life_base, col, row);
            cstate = clife_get_cell(life, col, row);
            expected_in_update = !cstate_cmp != !cstate;
            pt_state = find_point_in_updates(col, row, updates, update_len);
            if (expected_in_update && pt_state == NULL) return 1;
            if (!expected_in_update && pt_state != NULL) return 1;
            if (pt_state != NULL && !(pt_state->state) == !cstate_cmp)
                return 1;
        }
    }
    return 0;
} /* End validate_updates */

struct clife_point_state *find_point_in_updates(
    uint32_t x, uint32_t y,
    struct clife_point_state *updates, uint64_t update_len
) {
    for (uint64_t i = 0; i < update_len; i++) {
        if (updates->x == x && updates->y == y) return updates;
        updates++;
    }
    return NULL;
} /* End find_point_in_updates */
