#include "sim.h"
#include <getopt.h>

int main(int argc, char* argv[]) {
    // parse arguments
    if (argc < 2) {
        printf("no enough arguments.\n");
        exit(-1);
    }
    // initialize simulator
    static SIM sim;
    init_sim(&sim);
    sim.load(&sim, argv[1]);
    sim.run(&sim);
    return 0;
}
