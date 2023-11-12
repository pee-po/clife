#include <errno.h>
#include <string.h>

/*
 * Type definitions
 */

 /**
  * Structure representing parsed life state args
  * 
  * Structure representing parsed life state args, for list of arguments,
  * see parse_state_args.
  */
struct parsed_state_args {
    /** Char array of length num_bytes*2 given for serialisation */
    char *hexstr;
    /** Byte array of length num_bytes parsed from hexstr */
    uint8_t *bytes;
    uint32_t width; /**< Width of life object to allocate */
    uint32_t height; /**< Height of life object to allocate */
    /** Array of length num_points giving x coords of points to test */
    uint32_t *x_arr;
    /** Array of length num_points giving y coords of points to test */
    uint32_t *y_arr;
    size_t num_bytes; /**< Number of bytes in hexstr after parsing */
    size_t num_points; /**< Number of points in x_arr and y_arr */
};

/*
 * Function prototypes
 */

/**
 * Parse state args
 * 
 * Parse CLI args according to the specification:
 * * Hexstring to deserialise into a life object;
 * * Number of cells in width to allocate;
 * * Number of cells in height to allocate;
 * * Column coordinate of set cell no. 1;
 * * Row coordinate of set cell no. 1;
 * * Col of cell no. 2;
 * * Row of cell no. 2;
 * * Col of cell no. 3;
 * * ...
 * 
 * @param[in] argc int, number of CLI arguments;
 * @param[in] argv char**, array of CLI arguments starting with the command;
 * @param[out] parsed_state_args pointer to a struct with parsed args;
 */
struct parsed_state_args *parse_state_args(int argc, char **argv);

/**
 * Free parsed_state_args
 * 
 * @param args pointer to parsed_state_args to free;
 */
void free_state_args(struct parsed_state_args *args);

/**
 * Parse null-terminated hexstring
 * 
 * @param[in] hexstr null terminated char arr. Must have even length. Two
 * characters are intepreted as one byte.
 * @param[in] num_bytes a variable with this address will be set to number of
 * bytes in parsed array. Ownership remains with the caller.
 * @param[in] cp_hexstr if not NULL, variable at this address will be set to a
 * char array pointer. This array will be heap-allocated and initialised with
 * copy of hexstr. The size of this array is num_bytes*2. The ownership remains
 * with the caller.
 * @param[out] byte_array pointer to heap-allocated array. This array is
 * byte-interpretation of hexstr. The ownership is passed to the caller.
 */
uint8_t *parse_hexstr(char *hexstr, size_t *num_bytes, char **cp_hexstr);

/**
 * Parse two hex chars to byte.
 */
uint8_t hex2byte(char *start);

/*
 * Function definitions
 */
struct parsed_state_args *parse_state_args(int argc, char **argv) {
    if (argc < 4 || argc % 2) exit(1);
    struct parsed_state_args *args;
    args = malloc(sizeof *args);
    if (args == NULL) exit(2);

    /* Copy and parse hexstring */
    args->bytes = parse_hexstr(argv[1], &args->num_bytes, &(args->hexstr));
    if (args->bytes == NULL || args->hexstr == NULL) {
        free_state_args(args);
        exit(4);
    }

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
} /* End parse_state_args */

void free_state_args(struct parsed_state_args *args) {
    if (args != NULL) {
        free(args->hexstr);
        free(args->bytes);
        free(args->x_arr);
        free(args->y_arr);
        free(args);
    }
} /* End free_state_args */

uint8_t *parse_hexstr(char *hexstr, size_t *num_bytes, char **cp_hexstr) {
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
    
    if (cp_hexstr != NULL) {
        *cp_hexstr = malloc(num_chars + 1);
        if (*cp_hexstr != NULL) strncpy(*cp_hexstr, hexstr, num_chars + 1);
    }

    return bytes;
} /* End parse_hexstr */

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
} /* End hex2byte */
