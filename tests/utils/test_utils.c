#include <errno.h>

struct parsed_state_args {
    char *expected_hexstr;
    uint32_t width;
    uint32_t height;
    uint32_t *x_arr;
    uint32_t *y_arr;
    size_t num_points;
};

/* CLI parsing and cleanup */
struct parsed_state_args *parse_state_args(int argc, char **argv);
void free_state_args(struct parsed_state_args *args);

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

struct parsed_state_args *parse_state_args(int argc, char **argv) {
    if (argc < 6 || argc % 2) exit(1);
    struct parsed_state_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(2);

    args->expected_hexstr = argv[1];
    args->num_points = (argc - 4) / 2;
    args->x_arr = malloc(sizeof args->x_arr * args->num_points);
    args->y_arr = malloc(sizeof args->y_arr * args->num_points);
    if (args->x_arr == NULL || args->y_arr == NULL) {
        free_state_args(args);
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
        free_state_args(args);
        exit(2);
    }
    return args;
}

void free_state_args(struct parsed_state_args *args) {
    if (args != NULL) {
        free(args->expected_hexstr);
        free(args->x_arr);
        free(args->y_arr);
        free(args);
    }
}
