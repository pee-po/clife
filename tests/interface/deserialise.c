#include "clife_game.c"
#include "test_utils.c"
#include <string.h>
#include <errno.h>

struct parsed_args {
    uint8_t *bytes;
    uint32_t num_bytes;
    uint32_t width;
    uint32_t height;
};

struct parsed_args *parse_args(int argc, char **argv);
void free_args(struct parsed_args *args);

int main(int argc, char **argv) {
    struct parsed_args *args = parse_args(argc, argv);
    clife_t *life = new_clife(args->width, args->height);

    uint8_t *buffer;
    buffer = malloc(sizeof *buffer * args->num_bytes);
    if (buffer == NULL) return 1;

    clife_deserialise(life, args->bytes, args->num_bytes);
    delete_clife(life);
    return 0;
    clife_serialise(life, buffer, args->num_bytes);

    int cmp_res = memcmp(buffer, args->bytes, args->num_bytes);

    delete_clife(life);
    free_args(args);
    free(buffer);
    return cmp_res;
}

struct parsed_args *parse_args(int argc, char **argv) {
    if (argc != 4) exit(1);
    struct parsed_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(2);

    size_t num_chars = strlen(argv[1]);
    if (num_chars % 2) {
        free_args(args);
        exit(3);
    }
    size_t num_bytes = num_chars / 2;

    uint8_t *bytes;
    bytes = malloc(sizeof *bytes * num_bytes);
    if (bytes == NULL) {
        free_args(args);
        exit(2);
    }

    char *c = argv[1];
    for (size_t i = 0; i < num_bytes; i++) {
        bytes[i] = hex2byte(c);
        c += 2;
    }
    args->bytes = bytes;
    args->num_bytes = num_bytes;
    args->width = strtoul(argv[2], NULL, 10);
    args->height = strtoul(argv[3], NULL, 10);

    if (errno) {
        free_args(args);
        exit(2);
    }
    return args;
}

void free_args(struct parsed_args *args) {
    if (args != NULL) free(args->bytes);
    free(args);
}
