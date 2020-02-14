#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <chess/core.h>
#include <chrono>
#include <mutex>
#include <thread>
#include "time_format.h"

class player_settings;
class arbiter;


#define LOG_DEBUG_(X, Y) X << \
    pretty_time() << \
    " [DEBUG] [" << (player_color == chess::core::color::WHITE ? "WHITE" : "BLACK") << "] " << Y << '\n';

#define LOG_DEBUG(X) LOG_DEBUG_(std::cout, X);

class player {
    std::ostream& in;
    std::istream& out;
    std::chrono::system_clock::time_point last_saved_time = std::chrono::system_clock::time_point();
    std::thread thrd;
public:
    std::timed_mutex ready;
    std::timed_mutex time_to_play;
    std::timed_mutex has_played;
    std::chrono::milliseconds last_move_duration{0};
    chess::core::color player_color;

    player(chess::core::color c, std::ostream& in, std::istream& out) : player_color(c), in(in), out(out) {}

    void start_player(arbiter& arb, const std::string& options, const player_settings& psettings);

    void start_game();

    void stop_player();

    void set_position(const std::vector<std::string>& moves, const std::string& fen = "");

    void calculate_next_move(std::chrono::milliseconds white_time,
                             std::chrono::milliseconds black_time,
                             std::chrono::milliseconds white_increment,
                             std::chrono::milliseconds black_increment,
                             std::chrono::milliseconds movetime,
                             int max_depth);

    std::string get_next_move();

    void player_loop(arbiter& a, const player_settings& psettings);

};


#endif //CHESSENGINE_PLAYER_H
