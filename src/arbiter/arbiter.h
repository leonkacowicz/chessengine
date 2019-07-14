#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <vector>
#include <bits/unique_ptr.h>
#include "process.h"

using namespace std;

class arbiter {
    const string& white_player;
    const string& black_player;
    int white_time;
    int black_time;
    int increment;

    shared_ptr<process> white;
    shared_ptr<process> black;


public:
    arbiter(const string& white_player, const string& black_player, int total_seconds_per_player, int increment);
    void start_players();
    void start_game();
    void calculate_next_move(process*, vector<string> &);
};

#endif