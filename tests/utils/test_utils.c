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
