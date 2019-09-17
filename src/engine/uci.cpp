//
// Created by leon on 2019-09-08.
//

#include <cassert>
#include "uci.h"

uci_go_cmd::uci_go_cmd(const std::vector<std::string>& words) {
    using namespace std::chrono_literals;
    assert(words[0] == "go");
    is_valid = true;
    for (int i = 1; i < words.size(); i++) {
        if (words[i] == "wtime") {
            this->white_time = std::chrono::milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "btime") {
            this->black_time = std::chrono::milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "winc") {
            this->white_increment = std::chrono::milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "binc") {
            this->black_increment = std::chrono::milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "depth") {
            this->max_depth = std::stoi(words[i + 1]);
        }
        else if (words[i] == "nodes") {
            this->max_nodes = std::stoi(words[i + 1]);
        }
        else if (words[i] == "movetime") {
            this->move_time = std::chrono::milliseconds(std::stoi(words[i + 1]));
        }
        else if (words[i] == "infinite") {
            this->infinite = true;
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

