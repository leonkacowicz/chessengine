#include <iostream>
#include <unistd.h>
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

string getmove(istream& in) {
    string s;
    do {
        in >> s;
    } while (s != "bestmove");
    in >> s;
    return s;
}

void arbiter::start_game() {

    white->stdin << "ucinewgame" << endl;
    black->stdin << "ucinewgame" << endl;

    white->stdin << "position startpos" << endl;
    white->stdin << "go wtime " << white_time << " btime " << black_time
                << " winc " << increment << " binc " << increment << endl;

    string whitemove = getmove(white->stdout);
    cout << "White moves " << whitemove << endl;

    black->stdin << "position startpos moves " << whitemove << endl;
    black->stdin << "go wtime " << white_time << " btime " << black_time
                 << " winc " << increment << " binc " << increment << endl;

    string blackmove = getmove(black->stdout);
    cout << "Black moves " << blackmove << endl;
}
