#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include "arbiter.h"


arbiter::arbiter(const string &white_player, const string &black_player, int total_seconds_per_player, int increment) :
        white_player(white_player), black_player(black_player),
        white_time(total_seconds_per_player), black_time(total_seconds_per_player),
        increment(increment) {

}

void arbiter::start_players() {

    white = make_shared<process>(white_player);
    black = make_shared<process>(black_player);
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

string getline_with_timeout(istream & in, int timeout_millis) {
    std::atomic<bool> finished(false);
    timed_mutex mutex;
    mutex.lock();
    string line;
    auto t = thread([&] {
        getline(in, line);
        finished = true;
        mutex.unlock();
    });
    auto handle = t.native_handle();
    t.detach();
    if (mutex.try_lock_for(std::chrono::milliseconds(timeout_millis))) {
        mutex.unlock();
        return line;
    }
    pthread_cancel(handle);
    mutex.unlock();
    return "####";
}

string getmove(istream &in, string color_to_log) {
    string line;
    do {
        line = getline_with_timeout(in, 1000000);
        cout << "[DEBUG] [" << color_to_log << "] " << line << endl;
    } while (line.rfind("bestmove ", 0) != 0);

    stringstream ss(line.substr(9));
    string bestmove;
    ss >> bestmove;
    return bestmove;
}

void arbiter::calculate_next_move(process* player, vector<string>& moves) {

    player->stdin << "position startpos";
    if (!moves.empty()) {
        player->stdin << " moves ";
        for (auto move = begin(moves); move != end(moves); move++) {
            player->stdin << " " << *move;
        }
    }
    player->stdin << endl;
    player->stdin << "go wtime " << white_time << " btime " << black_time
                 << " winc " << increment << " binc " << increment << endl;
}

void arbiter::start_game() {

    white->stdin << "ucinewgame" << endl;
    black->stdin << "ucinewgame" << endl;

    white->stdin << "setoption name Threads value 8" << endl;

    vector<string> moves;

    calculate_next_move(white.get(), moves);

    string whitemove = getmove(white->stdout, "WHITE");
    cout << "White moves " << whitemove << endl;

    moves.push_back(whitemove);

    calculate_next_move(black.get(), moves);

    string blackmove = getmove(black->stdout, "BLACK");
    cout << "Black moves " << blackmove << endl;
    moves.push_back(blackmove);

    for (int i = 2; i < 2000; i++) {
        cout << i << endl;
        calculate_next_move(white.get(), moves);
        whitemove = getmove(white->stdout, "WHITE");
        if (whitemove == "(none)") break;
        cout << "White moves " << whitemove << endl;
        moves.push_back(whitemove);

        calculate_next_move(black.get(), moves);
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
