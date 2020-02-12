//
// Created by leon on 2019-11-03.
//

#ifndef CHESSENGINE_SETTINGS_H
#define CHESSENGINE_SETTINGS_H

#include <chrono>

struct player_settings {
    std::string executable;
    std::string input_filename;
    std::chrono::milliseconds initial_time;
    std::chrono::milliseconds time_increment;
    std::chrono::milliseconds move_time;
    int max_depth;
    int priority;
};

struct game_settings {
    player_settings white_settings;
    player_settings black_settings;
    std::string initial_position;
    std::string output_location;
    bool valid;
    bool verbose;
};

#endif //CHESSENGINE_SETTINGS_H
