//
// Created by leon on 2020-01-31.
//

#include <chess/uci/uci.h>
#include <cassert>
#include <sstream>

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

cmd_position chess::uci::parse_cmd_position(const std::vector<std::string>& tokens) {
    cmd_position command;
    command.is_valid = false;
    if (tokens.size() <= 1 || tokens[0] != "position") return command;
    int moves_pos = 0;
    if (tokens[1] == "startpos") {
        command.initial_position = "startpos";
        moves_pos = 2;
    } else {
        std::stringstream ss;
        int fen_start = 1;
        if (tokens[1] == "fen") fen_start = 2;
        ss << tokens[fen_start];
        for (int i = fen_start + 1; i < tokens.size(); moves_pos = ++i)
            if (tokens[i] == "moves") break;
            else ss << " " << tokens[i];
        command.initial_position = ss.str();
    }
    for (int i = moves_pos + 1; i < tokens.size(); i++) command.moves.push_back(tokens[i]);
    command.is_valid = true;
    return command;
}

cmd_info chess::uci::parse_cmd_info(const std::string& cmdline) {
    cmd_info cmd;
    cmd.is_valid = false;
    std::stringstream ss(cmdline);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return chess::uci::parse_cmd_info(tokens);
}

cmd_info chess::uci::parse_cmd_info(const std::vector<std::string>& tokens) {
    cmd_info cmd;
    cmd.is_valid = false;
    if (tokens[0] != "info") return cmd;
    cmd.is_valid = true;
    try {
        for (int i = 1; i < tokens.size() - 1; i++) {
            if (tokens[i] == "depth") {
                cmd.depth = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "score") {
                if (tokens.size() <= i + 2) {
                    cmd.is_valid = false;
                    return cmd;
                }
                if (tokens[i + 1] == "mate") cmd.score_mate = true;
                cmd.score = std::stoi(tokens[i + 2]);
                cmd.score_informed = true;
                i += 2;
                continue;
            }
            if (tokens[i] == "nps") {
                cmd.depth = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "multipv") {
                cmd.multipv = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "time") {
                cmd.time = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "seldepth") {
                cmd.seldepth = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "nodes") {
                cmd.nodes = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
            if (tokens[i] == "tbhits") {
                cmd.tbhits = std::stoi(tokens[i + 1]);
                i++;
                continue;
            }
        }
    } catch (const std::exception& e) {
        cmd.is_valid = false;
    }
    return cmd;
}
