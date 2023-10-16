#include "clife_game.c"
#include "test_utils.c"
#include <stdio.h>
#include <string.h>

#define BUFF_LEN 1024

/* Compare a buffer as produced by serialisation to a hexstring */
int compare_byte_hex(uint8_t *byte_buff, char *hex_buff, size_t buff_len);
/* helper for above */
uint8_t hex2byte(char *start);

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
    if (num_bytes*2 != strlen(args->expected_hexstr)) return 1;

    return compare_byte_hex(buffer, args->expected_hexstr, num_bytes);
}

int compare_byte_hex(uint8_t *byte_buff, char *hex_buff, size_t buff_len) {
    uint8_t byte_val, hex_val;
    for (size_t i = 0; i < buff_len; i++) {
        byte_val = *(byte_buff + i);
        hex_val = hex2byte(hex_buff);
        if (hex_val != byte_val) {
            fprintf(stderr, "@:%lu: %X != %X", i, byte_val, hex_val);
            return 1;
        }
        hex_buff += 2;
    }
    return 0;
}
