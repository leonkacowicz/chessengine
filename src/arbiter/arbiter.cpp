#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chess/board.h>
#include <algorithm>
#include <chess/move_gen.h>
#include <chess/fen.h>
#include "arbiter.h"
#include "player.h"

using namespace chess::core;

arbiter::arbiter(player& white_player,
                 player& black_player,
                 const game_settings& settings) :
        white(white_player),
        black(black_player),
        white_time(settings.white_settings.initial_time),
        black_time(settings.black_settings.initial_time),
        settings(settings) {
}

void arbiter::start_players() {
    this->start_players("", "");
}

void arbiter::start_players(const std::string & white_options, const std::string & black_options) {
    white.start_player(*this, white_options, settings.white_settings);
    black.start_player(*this, black_options, settings.black_settings);

    std::cout << "[DEBUG] Engines started" << std::endl;
}

constexpr auto equals(const std::string& s) {
    return [&] (const move& m) { return to_long_move(m) == s; };
}

board arbiter::get_initial_board() {
    std::stringstream str(settings.initial_position);
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
        initial_pos_fen = fen.str();
        b = chess::core::fen::board_from_fen(initial_pos_fen);
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
    white.ready.lock();
    black.ready.lock();

    white.has_played.lock();
    black.has_played.lock();

    board b = get_initial_board();

    int i = 0;
    bool player_won[2] = {false, false};
    int half_move_clock = 0;
    board last_positions[100];
    while(true) {
        ++i;
        std::cout << i << std::endl;
        if (settings.verbose) b.print();
        const bool white_to_play = b.side_to_play == WHITE;
        auto& current_time = white_to_play ? white_time : black_time;
        auto& current_player = white_to_play ? white : black;
        const auto& current_settings = white_to_play ? settings.white_settings : settings.black_settings;

        std::string side(white_to_play ? "White" : "Black");

        const std::vector<move> legal_moves = move_gen(b).generate();
        if (legal_moves.empty()) {
            bool check = b.under_check(b.side_to_play);
            if (check) {
                std::cout << "CHECKMATE" << std::endl;
                player_won[opposite(b.side_to_play)] = true;
            } else {
                std::cout << "STALEMATE" << std::endl;
            }
            break;
        }

        current_player.time_to_play.unlock();
        std::chrono::milliseconds wait_time(0);
        if (current_settings.move_time > 0ms) wait_time = current_settings.move_time;
        if (current_time > 0ms) wait_time = std::min(wait_time, current_time);

        if (wait_time > 0ms) {
            std::cout << "Wait for player move for " << wait_time.count() << "ms\n";
            if (current_player.has_played.try_lock_for(wait_time + 50ms)) {
                // all good
            } else {
                player_won[opposite(b.side_to_play)] = true;
                std::cout << side << " lost on time\n";
                break;
            }
        } else {
            std::cout << "Wait for player move\n";
            current_player.has_played.lock();
        }
        if (i > 2) {
            auto move_duration = duration_cast<milliseconds>(current_player.last_move_duration);
            std::cout << side << " took " << move_duration.count() << "ms." << std::endl;

            if (current_settings.initial_time.count() > 0) {
                current_time -= move_duration;
                current_time += current_settings.time_increment;

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
    white.stop_player();
    black.stop_player();

    int k = 0;
    std::stringstream pgn_output;

    if (!initial_pos_fen.empty()) {
        pgn_output << "[FEN \"" << initial_pos_fen << "\"]" << std::endl << std::endl;
    }

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
    if (!settings.output_location.empty()) {
        std::ofstream(settings.output_location) << pgn_output.str();
    }
}

