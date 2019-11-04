#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <vector>
#include <bits/unique_ptr.h>
#include <thread>
#include <mutex>
#include <board.h>
#include "settings.h"
#include "process.h"
#include "player.h"

struct mutexes {
    std::timed_mutex ready;
    std::timed_mutex time_to_play;
    std::timed_mutex has_played;
};

class arbiter {
    const std::string initial_pos;
    std::chrono::milliseconds white_time;
    std::chrono::milliseconds black_time;
    std::chrono::milliseconds white_increment;
    std::chrono::milliseconds black_increment;
    std::chrono::milliseconds white_move_time;
    std::chrono::milliseconds black_move_time;

    player& white;
    player& black;
    std::thread white_thread;
    std::thread black_thread;
    mutexes white_mutexes;
    mutexes black_mutexes;

    const bool verbose;
    bool game_finished = false;

    std::vector<std::string> moves;
    std::vector<std::string> pgn_moves;

    void player_loop(player& p, mutexes &m);
    void player_loop(player& p, mutexes &m, std::chrono::milliseconds move_time);
    board get_initial_board();
public:
    arbiter(player& white_player, player& black_player, const game_settings & settings);
    arbiter(player& white_player, player& black_player, std::chrono::milliseconds initial_time, std::chrono::milliseconds increment, bool verbose);
    void start_players();
    void start_players(const std::string & white_options, const std::string & black_options);
    void start_game();
};

#endif