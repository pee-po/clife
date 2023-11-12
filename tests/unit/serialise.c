#include "clife_game.c"
#include "test_utils.c"
#include <string.h>

#define BUFF_LEN 1024 /**< serialisation buffer size */

/**
 * Unit test life object serialisation
 *
 * This test is implementation-dependent. It tests serialisation of known life
 * object. Takes state_args (see test_utils.c); Allocates life object according
 * to width and height; Sets points according to (x_arr, y_arr); Serialises and
 * compares the outcome to parsed hexstr.
 */
int main(int argc, char **argv) {
    struct parsed_state_args *args = parse_state_args(argc, argv);

    clife_t *life = new_clife(args->width, args->height);
    size_t offset;
    uint32_t *x_arr = args->x_arr;
    uint32_t *y_arr = args->y_arr;
    for (size_t i = 0; i < args->num_points; i++) {
        /* 
         * Point setting depends on board implementation and is not portable
         * other tests may depend on serisalisation but at some point there
         * needs to be a implementation dependent test. This is it.
         */
        offset = x_arr[i] + life->width * y_arr[i];
        *(life->board + offset) = true;
    }

    uint8_t buffer[BUFF_LEN];
    size_t num_bytes = clife_serialise(life, buffer, BUFF_LEN);
    if (!num_bytes) return 1;
    int cmp_res = memcmp(buffer, args->bytes, num_bytes);
    if (num_bytes*2 != strlen(args->hexstr)) return 1;
    
    free_state_args(args);
    delete_clife(life);

    return cmp_res;
} /* End main */
