//
// Created by leon on 2019-09-08.
//

#ifndef CHESSENGINE_UCI_H
#define CHESSENGINE_UCI_H

#include <chrono>
#include <vector>
#include <string>

struct uci_go_cmd {
    std::chrono::milliseconds white_time{0};
    std::chrono::milliseconds black_time{0};
    std::chrono::milliseconds white_increment{0};
    std::chrono::milliseconds black_increment{0};
    std::chrono::milliseconds move_time{0};
    int max_depth = 0;
    int max_nodes = 0;
    std::vector<std::string> search_moves{};
    bool infinite = false;
    bool is_valid = false;

    uci_go_cmd(const std::vector<std::string>& words);
};


#endif //CHESSENGINE_UCI_H
