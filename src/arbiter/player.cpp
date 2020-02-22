//
// Created by leon on 13/07/19.
//

#include "player.h"
#include "arbiter.h"
#include "settings.h"

void player::start_player(arbiter& arb, const std::string& options, const player_settings& psettings) {
    if (!ready.try_lock()) throw std::runtime_error("ready mutex should be unlocked when start_player() is called.");
    if (!time_to_play.try_lock())
        throw std::runtime_error("time_to_play mutex should be unlocked when start_player() is called.");
    wrapper.send_uci();
    wrapper.wait_for_uciok();
    if (!options.empty()) in << options << std::endl;
    wrapper.send_isready();
    wrapper.wait_for_readyok();
    LOG_DEBUG("Engine Ready");

    thrd = std::thread([&] { player_loop(arb, psettings); });
}

void player::start_game() {
    wrapper.send_ucinewgame();
}

void player::stop_player() {
    time_to_play.unlock();
    thrd.join();
}

void player::set_position(const std::vector<std::string>& moves, const std::string& fen) {
    chess::uci::cmd_position pos(moves, fen);
    wrapper.send_position(pos);
}

void player::calculate_next_move(std::chrono::milliseconds white_time, std::chrono::milliseconds black_time,
                                 std::chrono::milliseconds white_increment, std::chrono::milliseconds black_increment,
                                 std::chrono::milliseconds movetime, int max_depth) {

    chess::uci::cmd_go cmd;
    cmd.white_time = white_time;
    cmd.black_time = black_time;
    cmd.white_increment = white_increment;
    cmd.black_increment = black_increment;
    cmd.move_time = movetime;
    cmd.max_depth = max_depth;
    cmd.is_valid = true;
    cmd.infinite = false;
    wrapper.send_go(cmd);
    last_saved_time = std::chrono::system_clock::now();
}

std::string player::get_next_move() {
    std::string line;
    std::stringstream buffer;
    while (std::getline(out, line)) {
        auto now = std::chrono::system_clock::now(); // must be very first thing after getting unblocked by std::getline
        if (!line.empty()) LOG_DEBUG_(buffer, line);

        if (line.substr(0, 9) == "bestmove ") {
            last_move_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_saved_time);
            std::stringstream ss(line.substr(9));
            std::string bestmove;
            ss >> bestmove;
            std::cout << buffer.str() << std::endl;
            return bestmove;
        }
    }
    LOG_DEBUG(buffer.str());
    LOG_DEBUG("=================EOF================");
    return "(none)";
}

void player::player_loop(arbiter& a, const player_settings& psettings) {
    //std::cout << "Starting player " << p.player_color << std::endl;
    ready.unlock();
    while (!a.game_finished) {
        //std::cout << "Waiting for player " << p.player_color << " turn" << std::endl;
        time_to_play.lock();
        if (a.game_finished) {
            //std::cout << "Game finished while player " << p.player_color << " waited to play" << std::endl;
            break;
        }
        //std::cout << "Time for player " << p.player_color << " to play" << std::endl;
        set_position(a.moves, a.initial_pos_fen);
        calculate_next_move(a.white_time, a.black_time, a.settings.white_settings.time_increment,
                            a.settings.black_settings.time_increment, psettings.move_time, psettings.max_depth);
        std::string player_move = get_next_move();
        std::cout.flush();
        a.moves.push_back(player_move);

        has_played.unlock();
        //std::cout << "Player " << p.player_color << " released play lock and aquiring wait lock" << std::endl;
    }
}
