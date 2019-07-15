#include <iostream>
#include "process.h"
#include "player.h"
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

    process white_proc(argv[1]);
    process black_proc(argv[2]);
    player white(WHITE, white_proc.stdin, white_proc.stdout);
    player black(BLACK, black_proc.stdin, black_proc.stdout);
    arbiter arb(white, black, atoi(argv[3]), atoi(argv[4]));

    arb.start_players();
    arb.start_game();
}

void print_usage() {
    cout << "Usage: chessarbiter /path/engine/white /path/engine/black "
            << "starting_time_in_seconds increment_seconds_per_move" << endl;

}
