#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <board.h>
#include <algorithm>
#include "arbiter.h"

arbiter::arbiter(player& white_player,
                 player& black_player,
                 const game_settings& settings) :
        white(white_player),
        black(black_player),
        white_time(settings.white_settings.initial_time),
        black_time(settings.black_settings.initial_time),
        white_increment(settings.white_settings.time_increment),
        black_increment(settings.black_settings.time_increment),
        white_move_time(settings.white_settings.move_time),
        black_move_time(settings.black_settings.move_time),
        verbose(settings.verbose) {
}

arbiter::arbiter(player& white_player,
                 player& black_player,
                 std::chrono::milliseconds initial_time,
                 std::chrono::milliseconds increment,
                 const bool verbose) :
        white(white_player),
        black(black_player),
        white_time(initial_time),
        black_time(initial_time),
        white_increment(increment),
        black_increment(increment),
        white_move_time(0),
        black_move_time(0),
        verbose(verbose) {
}

void arbiter::start_players() {
    this->start_players("", "");
}

void arbiter::start_players(const std::string & white_options, const std::string & black_options) {
    white.start_player(white_options);
    black.start_player(black_options);

    white_mutexes.ready.lock();
    black_mutexes.ready.lock();

    white_mutexes.time_to_play.lock();
    black_mutexes.time_to_play.lock();

    if (white_move_time.count() > 0)
        white_thread = std::thread([&] { player_loop(white, white_mutexes, white_move_time); });
    else
        white_thread = std::thread([&] { player_loop(white, white_mutexes); });

    if (black_move_time.count() > 0)
        black_thread = std::thread([&] { player_loop(black, black_mutexes, black_move_time); });
    else
        black_thread = std::thread([&] { player_loop(black, black_mutexes); });

    std::cout << "[DEBUG] Engines started" << std::endl;
}

auto equals(const std::string& s) {
    return [&] (const move& m) { return to_long_move(m) == s; };
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
    board last_positions[50];
    while(true) {
        std::cout << ++i << std::endl;
        if (verbose) b.print();
        mutexes& current = b.side_to_play == WHITE ? white_mutexes : black_mutexes;
        auto& current_time = b.side_to_play == WHITE ? white_time : black_time;
        auto increment = b.side_to_play == WHITE ? white_increment : black_increment;
        auto move_time = b.side_to_play == WHITE ? white_move_time : black_move_time;

        color side_color  = b.side_to_play;
        std::string side(b.side_to_play == WHITE ? "White" : "Black");

        const std::vector<move> legal_moves = b.get_legal_moves(side_color);
        if (legal_moves.empty()) {
            bool check = b.under_check(side_color);
            std::cout << side << " under check? " << check << std::endl;
            if (check) {
                std::cout << "CHECKMATE" << std::endl;
                player_won[opposite(b.side_to_play)] = true;
            } else {
                std::cout << "STALEMATE" << std::endl;
            }
            break;
        }

        auto time_before_move = std::chrono::system_clock::now();
        current.time_to_play.unlock();
        std::cout << "Wait for player move for " << move_time.count() << "ms\n";
        if (current.has_played.try_lock_for(move_time.count() > 0 ? move_time + std::chrono::milliseconds(10) : current_time)) {
            // all good
        } else {
            player_won[opposite(b.side_to_play)] = true;
            std::cout << side << " lost on time\n";
            break;
        }

        auto time_after_move = std::chrono::system_clock::now();
        if (i > 2) {
            auto move_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_after_move - time_before_move);
            std::cout << side << " took " << move_duration.count() << "ms." << std::endl;

            if (move_time.count() == 0) {
                current_time -= move_duration;
                current_time += increment;

                std::cout << "White time: "
                          << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() / 60 << "m "
                          << std::chrono::duration_cast<std::chrono::seconds>(white_time).count() % 60 << "s"
                          << std::endl;

                std::cout << "Black time: "
                          << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() / 60 << "m"
                          << std::chrono::duration_cast<std::chrono::seconds>(black_time).count() % 60 << "s"
                          << std::endl;
            }
        }

        auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), equals(moves.back()));
        if (move_found != legal_moves.end()) {
            if (b.resets_half_move_counter(*move_found)) half_move_clock = 0;
            else half_move_clock++;
            pgn_moves.push_back(b.move_in_pgn(*move_found, legal_moves));
            b.make_move(*move_found);
            if (half_move_clock >= 50) {
                std::cout << "DRAW BY 50-move RULE" << std::endl;
                break;
            } else {
                int repetition_count = 0;
                for (int k = 0; k < half_move_clock; k++) if (last_positions[k] == b)
                    repetition_count++;

                if (repetition_count >= 3) {
                    std::cout << "DRAW BY 3-FOLD-REPETITION" << std::endl;
                    break;
                }
                last_positions[half_move_clock] = b;
            }

        } else {
            std::cout << "Move " << moves.back() << " not found in list of legal moves!!" << std::endl;
            // player resigned
            std::cout << "RESIGNED/ILLEGAL MOVE" << std::endl;
            player_won[opposite(b.side_to_play)] = true;
            for (auto m : legal_moves) std::cout << to_long_move(m) << std::endl;
            break;
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
        if (k % 10 == 0) std::cout << std::endl;
        if (k % 2 == 0) std::cout << k/2 + 1 << ". ";
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
        p.calculate_next_move(white_time, black_time, white_increment, black_increment);
        std::string player_move = p.get_next_move();
        moves.push_back(player_move);

        m.has_played.unlock();
        //std::cout << "Player " << p.player_color << " released play lock and aquiring wait lock" << std::endl;
    }
}

void arbiter::player_loop(player& p, mutexes &m, std::chrono::milliseconds move_time) {
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
        p.calculate_next_move(move_time);
        std::string player_move = p.get_next_move();
        moves.push_back(player_move);

        m.has_played.unlock();
        //std::cout << "Player " << p.player_color << " released play lock and aquiring wait lock" << std::endl;
    }
}
