#include <iostream>
#include "arbiter.h"

using namespace std;

void print_usage();

int main(int argc, char ** argv) {

    if (argc != 2) {
        print_usage();
        return -1;
    }

    arbiter arb("cat", "cat", 3 * 60, 1);

    arb.hello();
}

void print_usage() {
    cout << "Usage: chessarbiter /path/engine/white /path/engine/black "
            << "starting_time_in_seconds increment_seconds_per_move" << endl;

}
