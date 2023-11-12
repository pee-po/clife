#include "clife_game.c"
#include "test_utils.c"
#include <string.h>
#include <errno.h>

/**
 * Test life object deserialisation
 *
 * Tests deserialisation of known life object. Takes state_args (see
 * test_utils.c); Allocates life object according to width and height;
 * Deserialises according to parsed hexstr; Serialises again and compares the
 * outcome to parsed hexstr.
 */
int main(int argc, char **argv) {
    struct parsed_state_args *args = parse_state_args(argc, argv);
    clife_t *life = new_clife(args->width, args->height);

    uint8_t *buffer;
    buffer = malloc(sizeof *buffer * args->num_bytes);
    if (buffer == NULL) return 1;

    clife_deserialise(life, args->bytes, args->num_bytes);
    clife_serialise(life, buffer, args->num_bytes);

    int cmp_res = memcmp(buffer, args->bytes, args->num_bytes);

    delete_clife(life);
    free_state_args(args);
    free(buffer);
    return cmp_res;
} /* End main */
