#include "clife_game.h"
#include "test_utils.c"

#define BUFF_LEN 1024

bool is_point_in_arr(
    uint32_t x,
    uint32_t y,
    uint32_t *x_arr,
    uint32_t *y_arr,
    size_t length
);

int main(int argc, char **argv) {
    struct parsed_state_args *args = parse_state_args(argc, argv);
    uint32_t width = args->width;
    uint32_t height = args->height;
    bool cstate, estate;

    clife_t *life = new_clife(width, height);
    clife_deserialise(life, args->bytes, args->num_bytes);
    for (uint32_t col = 0; col < width; col++) {
        for (uint32_t row = 0; row < height; row++) {
            cstate = clife_get_cell(life, col, row);
            estate = is_point_in_arr(
                col, row,
                args->x_arr, args->y_arr,
                args->num_points
            );
            if (!cstate != !estate) return 1;
        }
    }
    return 0;
}

bool is_point_in_arr(
    uint32_t x,
    uint32_t y,
    uint32_t *x_arr,
    uint32_t *y_arr,
    size_t length
) {
    for (size_t i = 0; i < length; i++) {
        if (*(x_arr + i) == x && *(y_arr + i) == y) return true;
    }
    return false;
}