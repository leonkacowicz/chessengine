#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <vector>
#include <bits/unique_ptr.h>
#include <thread>
#include <mutex>
#include "process.h"
#include "player.h"

using namespace std;

struct mutexes {
    timed_mutex ready;
    timed_mutex time_to_play;
    timed_mutex has_played;
};

class arbiter {
    chrono::milliseconds white_time;
    chrono::milliseconds black_time;
    chrono::milliseconds increment;

    player& white;
    player& black;

    thread white_thread;
    thread black_thread;

    void player_loop(player& p, mutexes &m);

    mutexes white_mutexes;
    mutexes black_mutexes;

    bool game_finished = false;

    vector<string> moves;
public:
    arbiter(player& white_player, player& black_player, chrono::milliseconds initial_time, chrono::milliseconds increment);
    void start_players();
    void start_game();
};

#endif