#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <board.h>
#include <magic_bitboard.h>
#include <fstream>
#include "engine.h"
#include "uci.h"
#include "static_evaluator.h"
#include "nn_eval.h"

using std::stringstream;
using std::string;

std::vector<string> split(const string& input, const string& delimiter = " ") {
    auto start = 0U;
    auto end = input.find(delimiter);
    auto delim_len = delimiter.length();
    std::vector<string> ret;
    while (end != std::string::npos)
    {
        ret.push_back(input.substr(start, end - start));
        start = end + delim_len;
        end = input.find(delimiter, start);
    }

    ret.push_back(input.substr(start, end));
    return ret;
}

void print_illegal_start_sequence_message(const std::vector<string>& words, int pos) {
    std::cerr << "ERROR: Illegal move sequence from startpos:";
    for (int i = 2; i <= pos; i++) {
        std::cerr << " " << words[i];
    }
    std::cerr << std::endl;
}

board handle_position_cmd(const std::vector<string>& words) {
    assert(words[0] == "position");
    if (words[1] == "startpos") {
        board b;
        b.set_initial_position();
        auto iter = std::find(words.begin(), words.end(), "moves");
        if (iter == words.end()) return b;
        while (++iter != words.end()) {
            auto moves = b.get_legal_moves(b.side_to_play);
            auto move_found = std::find_if(moves.begin(), moves.end(), [&] (const move& m) {
                return to_long_move(m) == *iter;
            });
            if (move_found != moves.end()) {
                b.make_move(*move_found);
            } else {
                print_illegal_start_sequence_message(words, iter - words.begin() + 1);
                return b;
            }
        }
        return b;
    }
}

int main()
{
    chess::core::init_bitboards();
    chess::core::init_magic_bitboards();
    zobrist::init();

    std::unique_ptr<evaluator> eval = std::make_unique<static_evaluator>();
    std::unique_ptr<engine> eng = std::make_unique<engine>(*eval.get());
    board b;
    b.set_initial_position();
    while (!std::cin.eof()) {
        string input;
        std::getline(std::cin, input);
        auto words = split(input);

        if (words[0] == "uci") {
            std::cout << "uciok" << std::endl;
            std::cout.flush();
            continue;
        } else if (words[0] == "ready") {
            std::cout << "readyok" << std::endl;
            std::cout.flush();
            continue;
        } else if (words[0] == "position") {
            if (words[1] == "startpos") {
                b = handle_position_cmd(words);
            }
            continue;
        } else if (words[0] == "go") {
            uci_go_cmd cmd(words);
            if (b.get_legal_moves(b.side_to_play).empty()) {
                std::cerr << "no legal move found to be searched" << std::endl;
                std::cout << "bestmove (none)" << std::endl;
            } else {
                std::cout << "info calculating move time for " << cmd.move_time.count() << "ms\n";
                auto selected_move = eng->timed_search(b, cmd.move_time);
                std::cout << "bestmove " << to_long_move(selected_move) << std::endl;
                std::cout.flush();
            }
        } else if (words[0] == "print") {
            b.print();
        } else if (words[0] == "setoption" && words[1] == "name" && words[2] == "evaluator" && words[3] == "value") {
            std::cout << "info using file " << words[4] << std::endl;
            std::ifstream ifs(words[4]);
            eval = std::make_unique<nn_eval>(ifs);
            eng = std::make_unique<engine>(*eval.get());
            std::cout.flush();
        }
    }
    return 0;
}