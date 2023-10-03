#include "clife_game.c"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define BUFF_LEN 1024

struct parsed_args {
    char *expected_hexstr;
    uint32_t width;
    uint32_t height;
    uint32_t *x_arr;
    uint32_t *y_arr;
    size_t num_points;
};

/* CLI parsing and cleanup */
struct parsed_args *parse_args(int argc, char **argv);
void free_args(struct parsed_args *args);
/* Compare a buffer as produced by serialisation to a hexstring */
int compare_byte_hex(uint8_t *byte_buff, char *hex_buff, size_t buff_len);
/* helper for above */
uint8_t hex2byte(char *start);

int main(int argc, char **argv) {
    struct parsed_args *args = parse_args(argc, argv);

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


struct parsed_args *parse_args(int argc, char **argv) {
    if (argc < 6 || argc % 2) exit(1);
    struct parsed_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(2);

    args->expected_hexstr = argv[1];
    args->num_points = (argc - 4) / 2;
    args->x_arr = malloc(sizeof args->x_arr * args->num_points);
    args->y_arr = malloc(sizeof args->y_arr * args->num_points);
    if (args->x_arr == NULL || args->y_arr == NULL) {
        free_args(args);
        exit(2);
    }

    int argci = 4;
    int ind = 0;
    while (argci < argc) {
        args->x_arr[ind] = strtoul(argv[argci], NULL, 10);
        args->y_arr[ind] = strtoul(argv[argci + 1], NULL, 10);
        argci += 2;
        ind++;
    }
    args->width = strtoul(argv[2], NULL, 10);
    args->height = strtoul(argv[3], NULL, 10);

    if (errno) {
        free_args(args);
        exit(2);
    }
    return args;
}

void free_args(struct parsed_args *args) {
    if (args != NULL) {
        free(args->expected_hexstr);
        free(args->x_arr);
        free(args->y_arr);
        free(args);
    }
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

uint8_t hex2byte(char *start) {
    uint8_t byte = 0;
    for (int i = 0; i < 2; i++) {
        byte = byte << 4;
        char c = *start;
        if (c >= '0' && c <= '9') {
            byte += (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            byte += (c - 'A' + 10);
        } else {
            exit(1);
        }
        start++;
    }
    return byte;
}

