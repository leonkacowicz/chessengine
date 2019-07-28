#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>
#include <board.h>
#include <algorithm>
#include "arbiter.h"

arbiter::arbiter(player& white_player, player& black_player, std::chrono::milliseconds initial_time, std::chrono::milliseconds increment) :
        white(white_player), black(black_player),
        white_time(initial_time), black_time(initial_time),
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

    white_thread = std::thread([&] { player_loop(white, white_mutexes); });
    black_thread = std::thread([&] { player_loop(black, black_mutexes); });

    std::cout << "[DEBUG] Engines started" << std::endl;
}

auto equals(std::string s) {
    return [&] (const move& m) { return m.to_long_move() == s; };
}

void arbiter::start_game() {

    white.start_game();
    black.start_game();

    // lock again (only works after each thread has started and unlocked the thread_ready mutex
    white_mutexes.ready.lock();
    black_mutexes.ready.lock();

    white_mutexes.has_played.lock();
    black_mutexes.has_played.lock();

    board b;
    b.set_initial_position();

    int i = 0;
    while(true) {
        std::cout << ++i << std::endl;
        b.print();
        mutexes& current = b.side_to_play == WHITE ? white_mutexes : black_mutexes;
        auto& current_time = b.side_to_play == WHITE ? white_time : black_time;
        color side_color  = b.side_to_play;
        std::string side(b.side_to_play == WHITE ? "White" : "Black");

        auto time_before_move = std::chrono::system_clock::now();
        current.time_to_play.unlock();
        if (current.has_played.try_lock_for(std::chrono::milliseconds(current_time))) {
            // all good
        } else {
            std::cout << side << " lost on time\n";
            break;
        }

        auto time_after_move = std::chrono::system_clock::now();
        if (i > 2) {
            auto move_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after_move - time_before_move);
            current_time -= move_duration;
            std::cout << side << " took " << move_duration.count()
                << "ms. White time: " << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() / 60
                << ":" << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() % 60
                << "; Black time: " << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() / 60
                << ":" << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() % 60
                << std::endl;
        }

        std::cout << side << " moves " << moves.back() << std::endl;
        {
            const std::vector<move> legal_moves = b.get_legal_moves(side_color);
            auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), equals(moves.back()));
            if (move_found != legal_moves.end()) {
                b.make_move(*move_found);
            } else {
                std::cout << "Move " << moves.back() << " not found in list of legal moves!!" << std::endl;
                bool check = b.under_check(side_color);
                std::cout << side << " under check? " << check << std::endl;
                if (check && legal_moves.empty()) std::cout << "CHECKMATE" << std::endl;
                for (auto m : legal_moves) std::cout << m.to_long_move() << std::endl;
                break;
            }
        }
    }
    game_finished = true;
    white_mutexes.time_to_play.unlock();
    black_mutexes.time_to_play.unlock();
    white_thread.join();
    black_thread.join();

    int k = 0;
    for (auto move = begin(moves); move != end(moves); move++) {
        if (k % 2 == 0) std::cout << std::endl << k/2 + 1 << ". ";
        std::cout << *move << " ";
        k++;
    }
    std::cout << std::endl;
}

void arbiter::player_loop(player& p, mutexes &m) {
    //std::cout << "Starting player " << p.player_color << std::endl;
    m.ready.unlock();
    while (!game_finished) {
        //std::cout << "Waiting for player " << p.player_color << " turn" << std::endl;
        m.time_to_play.lock();
        if (game_finished) {
            //std::cout << "Game finished while player " << p.player_color << " waited to play" << std::endl;
            break;
        }
        //std::cout << "Time for player " << p.player_color << " to play" << std::endl;
        p.set_position(moves);
        p.calculate_next_move(white_time, black_time, increment, increment);
        std::string player_move = p.get_next_move();
        moves.push_back(player_move);

        m.has_played.unlock();
        //std::cout << "Player " << p.player_color << " released play lock and aquiring wait lock" << std::endl;
    }
}
