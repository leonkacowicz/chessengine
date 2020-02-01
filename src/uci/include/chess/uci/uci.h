//
// Created by leon on 2020-01-31.
//

#ifndef CHESSENGINE_CHESSUCI_H
#define CHESSENGINE_CHESSUCI_H


#include <chrono>
#include <utility>
#include <vector>
#include <string>
#include <ostream>

namespace chess::uci {
struct cmd_go {
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

    cmd_go(std::vector<std::string> words = {});
    friend std::ostream& operator<<(std::ostream& os, const cmd_go& cmd);
};

struct cmd_basic {
    std::string name;

    cmd_basic(std::string name): name(std::move(name)) {}
    friend std::ostream& operator<<(std::ostream& os, const cmd_basic& cmd);
};

struct cmd_position {
    std::string initial_position;
    std::vector<std::string> moves;
    friend std::ostream& operator<<(std::ostream& os, const cmd_position& cmd);
};


}

#endif //CHESSENGINE_UCI_H
