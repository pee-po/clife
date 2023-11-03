#include "clife_game.h"
#include "test_utils.c"
#include <string.h>

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

struct parsed_args *parse_args(int argc, char **argv);
void free_args(struct parsed_args *args);
int validate_updates(
    clife_t *life_base, clife_t* life,
    struct clife_point_state *updates, uint64_t update_len
);
struct clife_point_state *find_point_in_updates(
    uint32_t x, uint32_t y,
    struct clife_point_state *updates, uint64_t update_len
);

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
}

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
}

void free_args(struct parsed_args *args) {
    if (args != NULL) {
        free(args->bytes_1);
        free(args->bytes_2);
        free(args);
    }
}

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
}

struct clife_point_state *find_point_in_updates(
    uint32_t x, uint32_t y,
    struct clife_point_state *updates, uint64_t update_len
) {
    for (uint64_t i = 0; i < update_len; i++) {
        if (updates->x == x && updates->y == y) return updates;
        updates++;
    }
    return NULL;
}
