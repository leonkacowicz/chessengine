#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include "arbiter.h"

arbiter::arbiter(player& white_player, player& black_player, int total_seconds_per_player, int increment) :
        white(white_player), black(black_player),
        white_time(total_seconds_per_player), black_time(total_seconds_per_player),
        increment(increment)
     {

}

void arbiter::start_players() {
    white.start_player();
    black.start_player();

    white_mutexes.ready.lock();
    black_mutexes.ready.lock();

    white_mutexes.time_to_play.lock();
    black_mutexes.time_to_play.lock();

    white_thread = thread([&] { player_loop(white, white_mutexes); });
    black_thread = thread([&] { player_loop(black, black_mutexes); });

//    white_thread.detach();
//    black_thread.detach();

    cout << "[DEBUG] Engines started" << endl;
}

void arbiter::start_game() {

    white.start_game();
    black.start_game();

    // lock again (only works after each thread has started and unlocked the thread_ready mutex
    white_mutexes.ready.lock();
    black_mutexes.ready.lock();

    white_mutexes.has_played.lock();
    black_mutexes.has_played.lock();

    auto last_time_point = chrono::system_clock::now();

    int i = 0;
    while(true) {
        cout << ++i << endl;

        white_mutexes.time_to_play.unlock();
        if (white_mutexes.has_played.try_lock_for(chrono::milliseconds(white_time))) {
            // all good
        } else {
            // lost on time
            break;
        }
        if (moves.back() == "(none)") break;

        if (i > 1) {
            auto last_move_time = chrono::system_clock::now();
            auto move_duration = chrono::duration_cast<chrono::milliseconds>(last_move_time - last_time_point).count();
            white_time -= move_duration;
            last_time_point = last_move_time;
            cout << "White took " << move_duration << " and now has " << white_time << endl;
        }

        cout << "White moves " << moves.back() << endl;

        black_mutexes.time_to_play.unlock();
        if (black_mutexes.has_played.try_lock_for(chrono::milliseconds(black_time))) {
            // all good
        } else {
            // black just lost on time
            break;
        }

        if (moves.back() == "(none)") break;
        if (i > 1) {
            auto last_move_time = chrono::system_clock::now();
            auto move_duration = chrono::duration_cast<chrono::milliseconds>(last_move_time - last_time_point).count();
            black_time -= move_duration;
            last_time_point = last_move_time;
            cout << "Black took " << move_duration << " and now has " << black_time << endl;
        }

    }
    game_finished = true;
    white_mutexes.time_to_play.unlock();
    black_mutexes.time_to_play.unlock();
    white_thread.join();
    black_thread.join();

    int k = 0;
    for (auto move = begin(moves); move != end(moves); move++) {
        if (k % 2 == 0) cout << endl << k/2 + 1 << ". ";
        cout << *move << " ";
        k++;
    }
    cout << endl;
}

void arbiter::player_loop(player& p, mutexes &m) {
    cout << "Starting player " << p.player_color << endl;
    m.ready.unlock();
    while (!game_finished) {
        cout << "Waiting for player " << p.player_color << " turn" << endl;
        m.time_to_play.lock();
        if (game_finished) {
            cout << "Game finished while player " << p.player_color << " waited to play" << endl;
            break;
        }
        cout << "Time for player " << p.player_color << " to play" << endl;
        p.set_position(moves);
        p.calculate_next_move(white_time, black_time, increment, increment);
        string player_move = p.get_next_move();
        moves.push_back(player_move);

        m.has_played.unlock();
        cout << "Player " << p.player_color << " released play lock and aquiring wait lock" << endl;
    }
}
