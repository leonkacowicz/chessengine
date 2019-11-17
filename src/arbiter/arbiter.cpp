#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <board.h>
#include <algorithm>
#include <move_gen.h>
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
        verbose(settings.verbose),
        output_file(settings.output_location),
        initial_pos(settings.initial_position) {
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
        verbose(verbose),
        initial_pos("startpos") {
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

constexpr auto equals(const std::string& s) {
    return [&] (const move& m) { return to_long_move(m) == s; };
}

board arbiter::get_initial_board() {
    std::stringstream str(initial_pos);
    std::string token;
    str >> token;
    board b;
    if (token.empty() || token == "startpos") {
        b.set_initial_position();
    } else if (token == "fen") {
        std::stringstream fen;
        for (int i = 0; i < 6; i++) {
            if (str.good()) {
                str >> token;
                if (i > 0) fen << " ";
                fen << token;
            }
        }

        b = board(fen.str());
    } else {
        throw std::invalid_argument("initial position provided must start with 'startpos' or 'fen'");
    }
    if (str.good()) {
        str >> token;
        if (token == "moves") while (str.good()) {
            str >> token;
            auto legal_moves = move_gen(b).generate();
            auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), equals(token));
            if (move_found == legal_moves.end()) {
                throw std::invalid_argument("initial position provided has illegal move: " + token);
            }
            pgn_moves.push_back(b.move_in_pgn(*move_found, legal_moves));
            moves.push_back(token);
            b.make_move(*move_found);
        }
    }
    return b;
}

void arbiter::start_game() {

    using namespace std::chrono_literals;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;

    white.start_game();
    black.start_game();

    // lock again (only works after each thread has started and unlocked the thread_ready mutex
    white_mutexes.ready.lock();
    black_mutexes.ready.lock();

    white_mutexes.has_played.lock();
    black_mutexes.has_played.lock();

    board b = get_initial_board();

    int i = 0;
    bool player_won[2] = {false, false};
    int half_move_clock = 0;
    board last_positions[100];
    while(true) {
        ++i;
        std::cout << i << std::endl;
        if (verbose) b.print();
        mutexes& current = b.side_to_play == WHITE ? white_mutexes : black_mutexes;
        auto& current_time = b.side_to_play == WHITE ? white_time : black_time;
        auto increment = b.side_to_play == WHITE ? white_increment : black_increment;
        auto move_time = b.side_to_play == WHITE ? white_move_time : black_move_time;

        color side_color  = b.side_to_play;
        std::string side(b.side_to_play == WHITE ? "White" : "Black");

        const std::vector<move> legal_moves = move_gen(b).generate();
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
        if (current.has_played.try_lock_for(move_time.count() > 0 ? move_time + 10ms : current_time)) {
            // all good
        } else {
            player_won[opposite(b.side_to_play)] = true;
            std::cout << side << " lost on time\n";
            break;
        }

        auto time_after_move = std::chrono::system_clock::now();
        if (i > 2) {
            auto move_duration = duration_cast<milliseconds>(time_after_move - time_before_move);
            std::cout << side << " took " << move_duration.count() << "ms." << std::endl;

            if (move_time.count() == 0) {
                current_time -= move_duration;
                current_time += increment;

                std::cout << "White time: "
                          << duration_cast<seconds>(white_time).count() / 60 << "m "
                          << duration_cast<seconds>(white_time).count() % 60 << "s"
                          << std::endl;

                std::cout << "Black time: "
                          << duration_cast<seconds>(black_time).count() / 60 << "m"
                          << duration_cast<seconds>(black_time).count() % 60 << "s"
                          << std::endl;
            }
        }

        auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), equals(moves.back()));
        if (move_found != legal_moves.end()) {
            if (b.resets_half_move_counter(*move_found)) half_move_clock = 0;
            else half_move_clock++;
            pgn_moves.push_back(b.move_in_pgn(*move_found, legal_moves));
            b.make_move(*move_found);
            if (half_move_clock >= 100) {
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
    std::stringstream pgn_output;
    if (!output_file.empty());
    for (auto move = begin(pgn_moves); move != end(pgn_moves); move++) {
        if (k % 10 == 0 && k > 0) pgn_output << std::endl;
        if (k % 2 == 0) pgn_output << k/2 + 1 << ". ";
        pgn_output << *move << " ";
        k++;
    }
    if (player_won[WHITE]) pgn_output << "\n1-0";
    else if (player_won[BLACK]) pgn_output << "\n0-1";
    else pgn_output << "\n1/2-1/2";
    pgn_output << std::endl;
    std::cout << pgn_output.str();
    if (!output_file.empty()) {
        std::ofstream(output_file) << pgn_output.str();
    }
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
