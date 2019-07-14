#include <iostream>
#include "arbiter.h"

using namespace std;

void print_usage();

int main(int argc, char ** argv) {
    if (argc != 5) {
        print_usage();
        //return -1;
    }

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    arbiter arb(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));

    arb.start_players();
    arb.start_game();
}

void print_usage() {
    cout << "Usage: chessarbiter /path/engine/white /path/engine/black "
            << "starting_time_in_seconds increment_seconds_per_move" << endl;

}
