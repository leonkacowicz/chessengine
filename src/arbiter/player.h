#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <color.h>
#include <chrono>
#include "time_format.h"

#define LOG_DEBUG(x) std::cout << \
    pretty_time() << \
    " [DEBUG] [" << (player_color == WHITE ? "WHITE" : "BLACK") << "] " << x << std::endl

class player {
    std::ostream& in;
    std::istream& out;
    std::chrono::system_clock::time_point last_saved_time = std::chrono::system_clock::time_point();
public:
    std::chrono::milliseconds last_move_duration{0};
    color player_color;

    player(color c, std::ostream& in, std::istream& out) : player_color(c), in(in), out(out) {}

    void start_player(const std::string& options) {
        in << "uci" << std::endl;
        in.flush();
        std::string line;
        do {
            getline(out, line);
            if (!line.empty())
                LOG_DEBUG(line);
        } while (line != "uciok");
        if (!options.empty()) in << options << std::endl;
        in << "isready" << std::endl;
        in.flush();
        while (out.good() && std::getline(out, line) && line != "readyok") {
            LOG_DEBUG(line);
        };
        LOG_DEBUG("Engine Ready");
    }

    void start_game() {
        in << "ucinewgame" << std::endl;
    }

    void set_position(const std::vector<std::string>& moves, const std::string& fen = "") {
        if (fen.empty())
            in << "position startpos";
        else
            in << "position fen " << fen;

        if (!moves.empty()) {
            in << " moves";
            for (auto move = begin(moves); move != end(moves); move++) {
                in << " " << *move;
            }
        }
        in << std::endl;
    }

    void calculate_next_move(std::chrono::milliseconds white_time,
                             std::chrono::milliseconds black_time,
                             std::chrono::milliseconds white_increment,
                             std::chrono::milliseconds black_increment,
                             std::chrono::milliseconds movetime,
                             int max_depth) {
        in << "go";
        if (white_time.count() > 0) in << " wtime " << white_time.count();
        if (black_time.count() > 0) in << " btime " << black_time.count();
        if (white_increment.count() > 0) in << " winc " << white_increment.count();
        if (black_increment.count() > 0) in << " binc " << black_increment.count();
        if (movetime.count() > 0) in << " movetime " << movetime.count();
        if (max_depth > 0) in << " depth " << max_depth;
        in << std::endl;
        in.flush();
        last_saved_time = std::chrono::system_clock::now();
    }

    std::string get_next_move() {
        std::string line;
        while (out.good()) {
            std::getline(out, line);
            auto now = std::chrono::system_clock::now(); // must be very first thing after getting unblocked by std::getline
            if (!line.empty()) LOG_DEBUG(line);

            if (line.substr(0, 9) == "bestmove ") {
                last_move_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_saved_time);
                std::stringstream ss(line.substr(9));
                std::string bestmove;
                ss >> bestmove;
                return bestmove;
            }
        }
        LOG_DEBUG(line);
        LOG_DEBUG("=================EOF================");
        return "(none)";
    }
};


#endif //CHESSENGINE_PLAYER_H
