#ifndef CHESSENGINE_PLAYER_H
#define CHESSENGINE_PLAYER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <color.h>

using namespace std;

class player {
    color c;
    ostream& in;
    istream& out;

public:

    //player(process& p): in(p.stdin), out(p.stdout) {}
    player(ostream& in, istream& out): in(in), out(out) {}

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

    void calculate_next_move(int white_time_millis, int black_time_millis, int white_increment, int black_increment) {
        in << "go wtime " << white_time_millis
            << " btime " << black_time_millis
            << " winc " << white_increment
            << " binc " << black_increment
            << endl;
    }

    string get_next_move() {
        string line;
        do {
            getline(out, line);
            cout << "[DEBUG] [" << (c == 1 ? "BLACK" : "WHITE") << "] " << line << endl;
        } while (line.rfind("bestmove ", 0) != 0);

        stringstream ss(line.substr(9));
        string bestmove;
        ss >> bestmove;
        return bestmove;
    }

};


#endif //CHESSENGINE_PLAYER_H
