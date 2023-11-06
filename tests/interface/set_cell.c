#include "clife_game.h"
#include "test_utils.c"

#define BUFF_LEN 1024 /**< serialisation buffer size */

/**
 * Test cell setter
 * 
 * Takes state_args (see test_utils.c). Initialises an empty life object
 * according to width and height; sets all points in (x_arr, y_arr); Serialises
 * and compares the serialised object to expectet state from parsed hexstr.
 */
int main(int argc, char **argv) {
    struct parsed_state_args *args = parse_state_args(argc, argv);
    
    clife_t *life = new_clife(args->width, args->height);
    for (size_t i = 0; i < args->num_points; i++) {
        clife_set_cell(
            life,
            *(args->x_arr + i),
            *(args->y_arr + i),
            true
        );
    }
    
    uint8_t buffer[BUFF_LEN];
    size_t num_bytes = clife_serialise(life, buffer, BUFF_LEN);
    if (args->num_bytes != num_bytes) return 1;
    int cmp_res = memcmp(buffer, args->bytes, args->num_bytes);

    return cmp_res;
} /* End main */
