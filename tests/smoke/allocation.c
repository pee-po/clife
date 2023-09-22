#include "clife_game.h"

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    clife_t *life = new_clife(3, 3);
    delete_clife(life);

    return 0;
}
