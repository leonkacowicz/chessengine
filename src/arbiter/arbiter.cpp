#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "arbiter.h"

void arbiter::hello() {
    std::cout << "Hello I'm arbiter" << std::endl;
}

arbiter::arbiter(const string &white_player, const string &black_player, int total_seconds_per_player, int increment) :
        white_player(white_player), black_player(black_player),
        white_time(total_seconds_per_player), black_time(total_seconds_per_player),
        increment(increment) {

}

void arbiter::start_players() {

    white = make_unique<process>(white_player);
    black = make_unique<process>(black_player);
    white->stdin << "uci" << endl;
    black->stdin << "uci" << endl;

    string line;
    do {
        getline(white->stdout, line);
        cout << "[DEBUG] [WHITE] " << line << endl;
    } while (line != "uciok");

    do {
        getline(black->stdout, line);
        cout << "[DEBUG] [BLACK] " << line << endl;
    } while (line != "uciok");

    cout << "[DEBUG] Engines started" << endl;
}

string getmove(istream &in, string color_to_log) {
    string line;
    do {
        getline(in, line);
        cout << "[DEBUG] [" << color_to_log << "] " << line << endl;
    } while (line.rfind("bestmove ", 0) != 0);

    stringstream ss(line.substr(9));
    string bestmove;
    ss >> bestmove;
    return bestmove;
}

void arbiter::start_game() {

    white->stdin << "ucinewgame" << endl;
    black->stdin << "ucinewgame" << endl;

    white->stdin << "setoption name Threads value 8" << endl;

    white->stdin << "position startpos" << endl;
    white->stdin << "go wtime " << white_time << " btime " << black_time
                 << " winc " << increment << " binc " << increment << endl;

    vector<string> moves;

    string whitemove = getmove(white->stdout, "WHITE");
    cout << "White moves " << whitemove << endl;

    moves.push_back(whitemove);

    black->stdin << "position startpos moves " << whitemove << endl;
    black->stdin << "go wtime " << white_time << " btime " << black_time
                 << " winc " << increment << " binc " << increment << endl;

    string blackmove = getmove(black->stdout, "BLACK");
    cout << "Black moves " << blackmove << endl;
    moves.push_back(blackmove);

    for (int i = 2; i < 2000; i++) {

        cout << i << endl;
        white->stdin << "position startpos moves";
        for (auto move = begin(moves); move != end(moves); move++) {
            white->stdin << " " << *move;
        }
        white->stdin << endl;
        white->stdin << "go wtime " << white_time << " btime " << black_time
                     << " winc " << increment << " binc " << increment << endl;

        whitemove = getmove(white->stdout, "WHITE");
        if (whitemove == "(none)") break;
        cout << "White moves " << whitemove << endl;
        moves.push_back(whitemove);

        black->stdin << "position startpos moves";
        for (auto move = begin(moves); move != end(moves); move++) {
            black->stdin << " " << *move;
        }
        black->stdin << endl;
        black->stdin << "go wtime " << white_time << " btime " << black_time
                     << " winc " << increment << " binc " << increment << endl;

        blackmove = getmove(black->stdout, "BLACK");
        if (blackmove == "(none)") break;
        cout << "Black moves " << blackmove << endl;
        moves.push_back(blackmove);
    }

    int k = 0;
    for (auto move = begin(moves); move != end(moves); move++) {
        if (k % 2 == 0) cout << endl << k/2 + 1 << ". ";
        cout << *move << " ";
        k++;
    }
    cout << endl;
}
