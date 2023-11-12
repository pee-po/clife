#include "clife_game.h"
#include <errno.h>

/**
 * Smoke test clife allocation and deallocation
 */
int main(int argc, char **argv);

/**
 * Parse args for allocation smoke test.
 * 
 * Parse arguments: width and height for life allocation.
 */
void parse_args(int argc, char **argv, uint32_t *width, uint32_t *height);

int main(int argc, char **argv) {
    uint32_t width, height;
    parse_args(argc, argv, &width, &height);

    clife_t *life = new_clife(width, height);
    delete_clife(life);

    return 0;
} /* End main */

void parse_args(int argc, char **argv, uint32_t *width, uint32_t *height) {
    if (argc != 3) exit(1);
    *width = strtoul(argv[1], NULL, 10);
    *height = strtoul(argv[2], NULL, 10);

    if (errno) exit(2);
} /* Enda parse_args */
