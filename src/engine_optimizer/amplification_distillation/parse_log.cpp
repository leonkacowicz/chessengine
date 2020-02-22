//
// Created by leonkaco on 2/5/20.
//

#include <string>
#include <chess/core.h>
#include <iostream>
#include <sstream>
#include <chess/board.h>
#include <chess/game.h>
#include <chess/uci/uci.h>
#include <chess/fen.h>
#include <chess/move_gen.h>
#include <algorithm>
#include <chess/engine/nn_eval.h>

using namespace chess::core;

struct parsed_log {
    std::vector<std::string> detected_initial_pos{"position"};
    std::vector<std::string> moves;
    std::vector<int> evaluations;
};

void operator<< (std::vector<std::string>& vec, std::stringstream& ss) {
    std::string token;
    ss >> token;
    vec.push_back(token);
}

parsed_log parse_log(std::istream& in) {
    parsed_log output;
    std::string line;
    int last_evaluation = 0;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string token;
        ss >> token;
        if (token == "--initial-pos") {
            ss >> token; // = equals sign
            while (!ss.eof()) output.detected_initial_pos << ss;
        } else if (token == "CHECKMATE" || token == "STALEMATE" || token == "DRAW" || token == "RESIGNED/ILLEGAL" || token == "RESIGNED") {
            break;
        } else {
            while (!ss.eof()) {
                ss >> token;
                if (token == "info") {
                    while (!ss.eof()) {
                        ss >> token;
                        if (token == "score") {
                            ss >> token;
                            if (token == "cp") {
                                ss >> last_evaluation;
                            } else {
                                ss >> last_evaluation;
                                if (last_evaluation < 0) last_evaluation = -10000 - last_evaluation;
                                else last_evaluation = 10000 - last_evaluation;
                            }
                        }
                    }
                } else if (token == "bestmove") {
                    ss >> token;
                    output.moves.push_back(token);
                    output.evaluations.push_back(last_evaluation);
                }
            }
        }
    }
    return output;
}

chess::core::game get_initial_game(const chess::uci::cmd_position& initialpos) {
    chess::core::board b;
    if (initialpos.initial_position == "startpos") b.set_initial_position();
    else b = chess::core::fen::board_from_fen(initialpos.initial_position);
    chess::core::game g(b);
    for (int i = 0; i < initialpos.moves.size(); i++) {
        auto legal = chess::core::move_gen(g.states.back().b).generate();
        auto found = std::find_if(legal.begin(), legal.end(), [&](const chess::core::move& m) { return chess::core::to_long_move(m) == initialpos.moves[i]; });
        if (found == legal.end()) {
            throw std::runtime_error("Error: invalid initial position detected. Move " + initialpos.moves[i] + " is not legal.");
        } else {
            g.do_move(*found);
        }
    }
    return g;
}

void add_training_sample(const board& b, int value, nn_eval& eval, std::ostream& os) {
    board board_ = b.side_to_play == BLACK ? b.flip_colors() : b;
    eval.fill_input_vector(board_);
    double y = double(std::min(std::max(value, -10000), 10000)) / 10000.0;
    os << y;
    for (int i = 0; i < eval.input_vector.size(); i++) os << "," << eval.input_vector[i];
    os << std::endl;
}


int main() {
    chess::core::init();
    parsed_log log = parse_log(std::cin);
    auto initialpos = chess::uci::parse_cmd_position(log.detected_initial_pos);
    chess::core::game g = get_initial_game(initialpos);
    chess::neural::mlp net(0, {nn_eval::INPUT_SIZE, 1});
    nn_eval eval(net);

    for (int i = 0; i < log.moves.size(); i++) {
        add_training_sample(g.states.back().b, log.evaluations[i], eval, std::cout);
        auto legal = chess::core::move_gen(g.states.back().b).generate();
        auto found = std::find_if(legal.begin(), legal.end(), [&](const chess::core::move& m) { return chess::core::to_long_move(m) == log.moves[i]; });
        if (found == legal.end()) {
            throw std::runtime_error("Error: invalid initial position detected. Move " + initialpos.moves[i] + " is not legal.");
        } else {
            g.do_move(*found);
        }
    }
}