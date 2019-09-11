//
// Created by leon on 2019-09-08.
//

#ifndef CHESSENGINE_UCI_H
#define CHESSENGINE_UCI_H

#include <chrono>
#include <vector>
#include <string>

struct uci_go_cmd {
    std::chrono::milliseconds white_time;
    std::chrono::milliseconds black_time;
    std::chrono::milliseconds white_increment;
    std::chrono::milliseconds black_increment;
    std::chrono::milliseconds move_time;
    int max_depth;
    int max_nodes;
    std::vector<std::string> search_moves;
    bool infinite;
    bool is_valid = false;

    uci_go_cmd(const std::vector<std::string>& words);
};


#endif //CHESSENGINE_UCI_H
