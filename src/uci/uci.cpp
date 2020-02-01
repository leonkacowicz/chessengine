//
// Created by leon on 2020-01-31.
//

#include <chess/uci/uci.h>
#include <cassert>

using namespace chess::uci;
using namespace std::chrono_literals;

std::ostream& operator<<(std::ostream& os, const cmd_go& cmd) {
    if (cmd.infinite) {
        os << "go infinite" << std::endl;
        return os;
    }
    os << "go";
    if (cmd.max_depth > 0) os << " depth " << cmd.max_depth;
    if (cmd.max_nodes > 0) os << " nodes " << cmd.max_nodes;
    if (cmd.move_time > 0ms) os << " movetime " << cmd.move_time.count();

    if (cmd.max_depth == 0 && cmd.max_nodes == 0 && cmd.move_time == 0ms) {
        os << " wtime" << cmd.white_time.count()
            << " btime " << cmd.black_time.count()
            << " winc " << cmd.white_increment.count()
            << " binc " << cmd.black_increment.count();
    }
    os << std::endl;
    return os;
}

cmd_go::cmd_go(std::vector<std::string> words) {
    using std::chrono::milliseconds;
    assert(words[0] == "go");
    is_valid = true;
    for (int i = 1; i < words.size(); i++) {
        if (words[i] == "wtime") {
            white_time = milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "btime") {
            black_time = milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "winc") {
            white_increment = milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "binc") {
            black_increment = milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "depth") {
            max_depth = std::stoi(words[i + 1]);
        }
        else if (words[i] == "nodes") {
            max_nodes = std::stoi(words[i + 1]);
        }
        else if (words[i] == "movetime") {
            move_time = milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "infinite") {
            infinite = true;
        }
    }

    if (infinite && (move_time > 0ms || max_depth > 0 || max_nodes > 0 || white_time > 0ms)) {
        is_valid = false;
    }

    if (std::max(white_time, black_time) > 0ms && std::min(white_time, black_time) <= 0ms) {
        is_valid = false;
    }

    if (white_time < 0ms || white_increment < 0ms || black_increment < 0ms || white_increment < 0ms || move_time < 0ms)
        is_valid = false;

    if (max_nodes < 0 || max_depth < 0)
        is_valid = false;
}

std::ostream& operator<<(std::ostream& os, const cmd_basic& cmd) {
    os << cmd.name << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const cmd_position& cmd) {
    os << "position " << cmd.initial_position;
    if (!cmd.moves.empty()) {
        os << " moves";
        for (const std::string& mv : cmd.moves) os << " " << mv;
    }
    os << std::endl;
    return os;
}
