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

int main(int argc, char **argv) {
    struct parsed_args *args = parse_args(argc, argv);
    if (args->num_bytes_1 != args->num_bytes_2) {
        free_args(args);
        return 1;
    }
    size_t num_bytes = args->num_bytes_1;

    /* Set up life and buffer */
    clife_t *life = new_clife(args->width, args->height);
    uint8_t *buff;
    buff = malloc(sizeof *buff * num_bytes);
    if (
        buff == NULL
        || life == NULL
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
    int cmp_res = memcmp(buff, args->bytes_2, num_bytes);
    
    /* Memory */
    free(buff);
    delete_clife(life);
    free_args(args);
    return cmp_res;
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
