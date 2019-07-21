#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <color.h>
#include <chrono>

using namespace std;

class player {
    ostream& in;
    istream& out;

public:
    color player_color;
    player(color c, ostream& in, istream& out): player_color(c), in(in), out(out) {}

    void start_player() {
        in << "uci" << endl;
        string line;
        do {
            getline(out, line);
            cout << "[DEBUG] [WHITE] " << line << endl;
        } while (line != "uciok");
    }

    void start_game() {
        in << "ucinewgame" << endl;
    }

    void set_position(const vector<string>& moves) {
        in << "position startpos";
        if (!moves.empty()) {
            in << " moves";
            for (auto move = begin(moves); move != end(moves); move++) {
                in << " " << *move;
            }
        }
        in << endl;
    }

    void calculate_next_move(chrono::milliseconds white_time,
                             chrono::milliseconds black_time,
                             chrono::milliseconds white_increment,
                             chrono::milliseconds black_increment) {
        in << "go wtime " << white_time.count()
            << " btime " << black_time.count()
            << " winc " << white_increment.count()
            << " binc " << black_increment.count()
            << endl;
    }

    string get_next_move() {
        string line;
        string info;
        do {
            getline(out, line);
            if (line.substr(0, 5) == "info ") info = line;
            else cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << line << endl;
        } while (line.rfind("bestmove ", 0) != 0);
        cout << "[DEBUG] [" << (player_color == 1 ? "BLACK" : "WHITE") << "] " << info << endl;

        stringstream ss(line.substr(9));
        string bestmove;
        ss >> bestmove;
        return bestmove;
    }

};


#endif //CHESSENGINE_PLAYER_H
