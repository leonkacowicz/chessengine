#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include "arbiter.h"

arbiter::arbiter(player& white_player, player& black_player, int total_seconds_per_player, int increment) :
        white(white_player), black(black_player),
        white_time(total_seconds_per_player), black_time(total_seconds_per_player),
        increment(increment) {
}

void arbiter::start_players() {
    white.start_player();
    black.start_player();

    cout << "[DEBUG] Engines started" << endl;
}

void arbiter::start_game() {

    white.start_game();
    black.start_game();

    vector<string> moves;

    int i = 1;
    while(true) {
        cout << i++ << endl;
        white.set_position(moves);
        white.calculate_next_move(white_time, black_time, increment, increment);
        string whitemove = white.get_next_move();
        if (whitemove == "(none)") break;
        cout << "White moves " << whitemove << endl;
        moves.push_back(whitemove);

        white.set_position(moves);
        black.calculate_next_move(white_time, black_time, increment, increment);
        string blackmove = black.get_next_move();
        if (blackmove == "(none)") break;
        cout << "Black moves " << blackmove << endl;
        moves.push_back(blackmove);
    }

    int k = 0;
    for (auto move = begin(moves); move != end(moves); move++) {
        if (k % 2 == 0) cout << endl << k/2 + 1 << ". ";
        cout << *move << " ";
        k++;
    }
    cout << endl;
}
