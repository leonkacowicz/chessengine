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
    std::string initial_pos_fen;
    std::chrono::milliseconds white_time;
    std::chrono::milliseconds black_time;
    const game_settings settings;

    player& white;
    player& black;
    std::thread white_thread;
    std::thread black_thread;
    mutexes white_mutexes;
    mutexes black_mutexes;

    bool game_finished = false;

    std::vector<std::string> moves;
    std::vector<std::string> pgn_moves;

    void player_loop(player& p, mutexes &m, const player_settings& psettings);
    board get_initial_board();
public:
    arbiter(player& white_player, player& black_player, const game_settings & settings);
    void start_players();
    void start_players(const std::string & white_options, const std::string & black_options);
    void start_game();
};

#endif