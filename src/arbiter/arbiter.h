#ifndef CHESSENGINE_ARBITER_H
#define CHESSENGINE_ARBITER_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include <chess/board.h>
#include "settings.h"
#include "process.h"

class player;

class arbiter {
public:
    std::string initial_pos_fen;
    std::chrono::milliseconds white_time;
    std::chrono::milliseconds black_time;
    const game_settings settings;

    player& white;
    player& black;

    bool game_finished = false;

    std::vector<std::string> moves;
    std::vector<std::string> pgn_moves;

    chess::core::board get_initial_board();

    arbiter(player& white_player, player& black_player, const game_settings& settings);

    void start_players();

    void start_players(const std::string& white_options, const std::string& black_options);

    void start_game();
};

#endif