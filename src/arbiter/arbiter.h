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

struct mutexes {
    std::timed_mutex ready;
    std::timed_mutex time_to_play;
    std::timed_mutex has_played;
};

class arbiter {
    std::chrono::milliseconds white_time;
    std::chrono::milliseconds black_time;
    std::chrono::milliseconds increment;

    player& white;
    player& black;

    std::thread white_thread;
    std::thread black_thread;

    void player_loop(player& p, mutexes &m);

    mutexes white_mutexes;
    mutexes black_mutexes;

    bool game_finished = false;

    std::vector<std::string> moves;
public:
    arbiter(player& white_player, player& black_player, std::chrono::milliseconds initial_time, std::chrono::milliseconds increment);
    void start_players();
    void start_game();
};

#endif