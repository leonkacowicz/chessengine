#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <bits/unique_ptr.h>
#include "process.h"

using namespace std;

class arbiter {
    const string& white_player;
    const string& black_player;
    int white_time;
    int black_time;
    int increment;

    unique_ptr<process> white;
    unique_ptr<process> black;

public:
    arbiter(const string& white_player, const string& black_player, int total_seconds_per_player, int increment);
    void start_players();
    void start_game();
    void hello();
};

#endif