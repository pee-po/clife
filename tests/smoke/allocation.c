#include "clife_game.h"
#include <errno.h>

void parse_args(int argc, char **argv, uint32_t *width, uint32_t *height);

int main(int argc, char **argv) {
    uint32_t width, height;
    parse_args(argc, argv, &width, &height);

    clife_t *life = new_clife(width, height);
    delete_clife(life);

    return 0;
}

void parse_args(int argc, char **argv, uint32_t *width, uint32_t *height) {
    if (argc != 3) exit(1);
    *width = strtoul(argv[1], NULL, 10);
    *height = strtoul(argv[2], NULL, 10);

    if (errno) exit(2);
}
