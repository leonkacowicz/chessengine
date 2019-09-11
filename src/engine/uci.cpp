//
// Created by leon on 2019-09-08.
//

#include <cassert>
#include "uci.h"

uci_go_cmd::uci_go_cmd(const std::vector<std::string>& words) {
    assert(words[0] == "go");

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
}

