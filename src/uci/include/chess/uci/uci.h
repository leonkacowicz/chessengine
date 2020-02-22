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

    cmd_go() {};
    cmd_go(std::vector<std::string> words);
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
    bool is_valid = false;
    friend std::ostream& operator<<(std::ostream& os, const cmd_position& cmd);
    cmd_position(std::vector<std::string> moves = {}, std::string initial_position = "startpos") : initial_position(initial_position.empty() ? "startpos" : std::move(initial_position)), moves(std::move(moves)), is_valid(true) {}
};

struct cmd_bestmove {
    std::string bestmove;
    std::string pondermove;
};

struct cmd_info {
    bool is_valid = false;
    int depth = -1;
    int seldepth = -1;
    int time = -1;
    int nodes = -1;
    std::vector<std::string> pv;
    int multipv = -1;
    int score;
    bool score_mate;
    bool score_informed = false;
    std::string currmove;
    int currmovenumber;
    int nps = -1;
    int tbhits = -1;
    std::string infostring;
};

cmd_position parse_cmd_position(const std::vector<std::string>& tokens);
cmd_info parse_cmd_info(const std::vector<std::string>& tokens);
cmd_info parse_cmd_info(const std::string& cmdline);

}

#endif //CHESSENGINE_UCI_H
