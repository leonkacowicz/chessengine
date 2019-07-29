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

    std::vector<std::string> pgn_moves;
    board b;
    b.set_initial_position();

    int i = 0;
    bool player_won[2] = {false, false};
    int half_move_clock = 0;
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
            player_won[opposite(b.side_to_play)] = true;
            std::cout << side << " lost on time\n";
            break;
        }

        auto time_after_move = std::chrono::system_clock::now();
        if (i > 2) {
            auto move_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after_move - time_before_move);
            current_time -= move_duration;
            current_time += increment;
            std::cout << side << " took " << move_duration.count()
                << "ms. White time: " << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() / 60
                << ":" << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() % 60
                << "; Black time: " << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() / 60
                << ":" << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() % 60
                << std::endl;
        }


        {
            const std::vector<move> legal_moves = b.get_legal_moves(side_color);
            auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), equals(moves.back()));
            if (move_found != legal_moves.end()) {
                if (b.resets_half_move_counter(*move_found)) half_move_clock = 0;
                else half_move_clock++;
                if (half_move_clock >= 50) {
                    std::cout << "DRAW BY 50-move RULE" << std::endl;
                    break;
                }
                pgn_moves.push_back(b.move_in_pgn(*move_found, legal_moves));
                b.make_move(*move_found);
            } else {
                std::cout << "Move " << moves.back() << " not found in list of legal moves!!" << std::endl;
                if (legal_moves.empty()) {
                    bool check = b.under_check(side_color);
                    std::cout << side << " under check? " << check << std::endl;
                    if (check) {
                        std::cout << "CHECKMATE" << std::endl;
                        player_won[opposite(b.side_to_play)] = true;
                    } else {
                        std::cout << "STALEMATE" << std::endl;
                    }
                } else {
                    // player resigned
                    std::cout << "RESIGNED" << std::endl;
                    player_won[opposite(b.side_to_play)] = true;
                    for (auto m : legal_moves) std::cout << m.to_long_move() << std::endl;
                }
                break;
            }
        }
        std::cout << side << " moves " << moves.back() << " / " << pgn_moves.back() << std::endl;
    }
    game_finished = true;
    white_mutexes.time_to_play.unlock();
    black_mutexes.time_to_play.unlock();
    white_thread.join();
    black_thread.join();

    int k = 0;
    for (auto move = begin(pgn_moves); move != end(pgn_moves); move++) {
        if (k % 2 == 0) std::cout << std::endl << k/2 + 1 << ". ";
        std::cout << *move << " ";
        k++;
    }
    if (player_won[WHITE]) std::cout << "\n1-0";
    else if (player_won[BLACK]) std::cout << "\n0-1";
    else std::cout << "\n1/2-1/2";
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
