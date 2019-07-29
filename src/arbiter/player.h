#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <color.h>
#include <chrono>

class player {
    std::ostream& in;
    std::istream& out;

public:
    color player_color;
    player(color c, std::ostream& in, std::istream& out): player_color(c), in(in), out(out) {}

    void start_player() {
        in << "uci" << std::endl;
        std::string line;
        do {
            getline(out, line);
            if (!line.empty())
                std::cout << "[DEBUG] [WHITE] " << line << std::endl;
        } while (line != "uciok");
    }

    void start_game() {
        in << "ucinewgame" << std::endl;
    }

    void set_position(const std::vector<std::string>& moves) {
        in << "position startpos";
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
                             std::chrono::milliseconds black_increment) {
        in << "go wtime " << white_time.count()
            << " btime " << black_time.count()
            << " winc " << white_increment.count()
            << " binc " << black_increment.count()
            << std::endl;
    }

    std::string get_next_move() {
        std::string line;
        std::string info;

        while (!out.eof()) {
            std::getline(out, line);

            if (line.substr(0, 5) == "info ") info = line;
            else if (!line.empty()) std::cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << line << std::endl;

            if (line.substr(0, 9) == "bestmove ") {
                std::cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << info << std::endl;
                std::cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] Move: " << line << std::endl;
                std::stringstream ss(line.substr(9));
                std::string bestmove;
                ss >> bestmove;
                return bestmove;
            }
        }
        std::cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << info << std::endl;
        std::cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << line << std::endl;
        return "(none)";
    }

};


#endif //CHESSENGINE_PLAYER_H
