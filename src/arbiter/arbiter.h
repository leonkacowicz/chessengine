#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <vector>
#include <bits/unique_ptr.h>
#include "process.h"
#include "player.h"

using namespace std;

class arbiter {
    int white_time;
    int black_time;
    int increment;

    player& white;
    player& black;


public:
    arbiter(player& white_player, player& black_player, int total_seconds_per_player, int increment);
    void start_players();
    void start_game();
    void calculate_next_move(process*, vector<string> &);
};

#endif