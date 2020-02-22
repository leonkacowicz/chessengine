//
// Created by leon on 2020-01-31.
//

#include <chess/uci/engine_wrapper.h>
#include <future>
#include <sstream>

using namespace std::chrono_literals;
using namespace chess::uci;

engine_wrapper::engine_wrapper(std::ostream& in, std::istream& out): std_in(in), std_out(out) {

}

void engine_wrapper::send_uci() {
    std_in << "uci" << std::endl;
    std_in.flush();
}

void engine_wrapper::wait_for_uciok() {
    while (true) {
        std::string str;
        std::getline(std_out, str);
        std::stringstream ss(str);
        if (str == "uciok") return;
        // to do: collect options info here
    }
}

void engine_wrapper::wait_for_readyok() {
    while (true) {
        std::string str;
        std::getline(std_out, str);
        std::stringstream ss(str);
        if (str == "readyok") return;
        // to do: collect options info here
    }
}

void engine_wrapper::send_option(const std::string& name, const std::string& value) {
    std_in << "setoption name " << name << " value " << value << std::endl;
}

void engine_wrapper::send_position(const cmd_position& cmd) {
    std_in << "position ";
    if (cmd.initial_position != "startpos") std_in << " fen ";
    std_in << cmd.initial_position;
    if (!cmd.moves.empty()) {
        std_in << " moves";
        for (const std::string& m : cmd.moves) std_in << " " << m;
    }
    std_in << std::endl;
}

void engine_wrapper::send_go(const cmd_go& cmd) {
    std_in << "go";
    if (cmd.infinite) {
        std_in << " infinite";
    } else {
        if (cmd.max_depth > 0) std_in << " depth " << cmd.max_depth;
        if (cmd.max_nodes > 0) std_in << " nodes " << cmd.max_nodes;
        if (cmd.move_time.count() > 0) std_in << " movetime " << cmd.move_time.count();
        if (cmd.white_time.count() > 0 || cmd.black_time.count() > 0)
            std_in << " wtime " << cmd.white_time.count() << " btime " << cmd.black_time.count()
                   << " winc " << cmd.white_increment.count() << " binc " << cmd.black_increment.count();
    }
    if (!cmd.search_moves.empty()) {
        std_in << " searchmoves";
        for (const std::string& m : cmd.search_moves) std_in << " " << m;
    }
    std_in << std::endl;
    std_in.flush();
}

void engine_wrapper::send_stop() {
    std_in << "stop" << std::endl;
    std_in.flush();
}

void engine_wrapper::send_isready() {
    std_in << "isready" << std::endl;
    std_in.flush();
}

void engine_wrapper::send_ucinewgame() {
    std_in << "ucinewgame" << std::endl;
}
