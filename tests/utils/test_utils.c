#include <errno.h>
#include <string.h>

struct parsed_state_args {
    char *hexstr;
    uint8_t *bytes;
    uint32_t num_bytes;
    uint32_t width;
    uint32_t height;
    uint32_t *x_arr;
    uint32_t *y_arr;
    size_t num_points;
};

/* CLI parsing and cleanup */
struct parsed_state_args *parse_state_args(int argc, char **argv);
void free_state_args(struct parsed_state_args *args);
uint8_t *parse_hexstr(char *hexstr, size_t *num_bytes);
uint8_t hex2byte(char *start);

struct parsed_state_args *parse_state_args(int argc, char **argv) {
    if (argc < 4 || argc % 2) exit(1);
    struct parsed_state_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(2);

    /* Copy hexstring */
    size_t num_chars = strlen(argv[1]);
    args->hexstr = malloc(sizeof *(args->hexstr) * num_chars + 1);
    if (num_chars % 2 || args->hexstr == NULL) {
        free_state_args(args);
        exit(3);
    }
    strncpy(args->hexstr, argv[1], num_chars + 1);

    size_t num_bytes;
    args->bytes = parse_hexstr(args->hexstr, &num_bytes);
    if (args->bytes == NULL) {
        free_state_args(args);
        exit(4);
    }
    args->num_bytes = num_bytes;

    /* Parse points */
    args->num_points = (argc - 4) / 2;
    if (args->num_points) {
        args->x_arr = malloc(sizeof args->x_arr * args->num_points);
        args->y_arr = malloc(sizeof args->y_arr * args->num_points);
        if (args->x_arr == NULL || args->y_arr == NULL) {
            free_state_args(args);
            exit(5);
        }

        int argci = 4;
        int ind = 0;
        while (argci < argc) {
            args->x_arr[ind] = strtoul(argv[argci], NULL, 10);
            args->y_arr[ind] = strtoul(argv[argci + 1], NULL, 10);
            argci += 2;
            ind++;
        }
    } else {
        args->x_arr = NULL;   
        args->y_arr = NULL;   
    }
    args->width = strtoul(argv[2], NULL, 10);
    args->height = strtoul(argv[3], NULL, 10);

    if (errno) {
        free_state_args(args);
        exit(6);
    }
    return args;
}

void free_state_args(struct parsed_state_args *args) {
    if (args != NULL) {
        free(args->hexstr);
        free(args->bytes);
        free(args->x_arr);
        free(args->y_arr);
        free(args);
    }
}

uint8_t *parse_hexstr(char *hexstr, size_t *num_bytes) {
    size_t num_chars = strlen(hexstr);
    if (num_chars % 2) return NULL;
    size_t num_bytes_in = num_chars / 2;
    uint8_t *bytes;
    bytes = malloc(sizeof *bytes * num_bytes_in);
    if (bytes == NULL) return NULL;

    char *c = hexstr;
    for (size_t i = 0; i < num_bytes_in; i++) {
        bytes[i] = hex2byte(c);
        c += 2;
    }
    if (num_bytes != NULL) *num_bytes = num_bytes_in;
    return bytes;
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
