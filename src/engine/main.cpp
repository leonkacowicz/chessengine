#include <iostream>
#include <string>
#include <algorithm>
#include <board.h>
#include "engine.h"
#include "mcts.h"

using std::stringstream;
using std::string;

int main()
{
    chess::core::init_bitboards();
    engine e;
    board b;
    b.set_initial_position();
    string line;
    while (!std::cin.eof()) {
        std::getline(std::cin, line);
        //std::cerr << "RECEIVED: " << line << std::endl;

        stringstream ss;
        ss << line;
        string word;
        ss >> word;
        //std::cerr << "Processing command: " << word << std::endl;
        if (word == "uci") {
            std::cout << "uciok" << std::endl;
            continue;
        } else if (word == "position") {
            ss >> word;
            if (word == "startpos") b.set_initial_position();

            while (!ss.eof() && word != "moves" && !word.empty())
                ss >> word;

            while (!ss.eof()) {
                ss >> word;
                auto legal_moves = b.get_legal_moves(b.side_to_play);
                auto move_found = std::find_if(legal_moves.begin(), legal_moves.end(), [&] (const move& m) {
                    return to_long_move(m) == word;
                });
                if (move_found == end(legal_moves)) {
                    std::cout << "ERROR: Illegal move informed" << std::endl;
                    break;
                } else {
                    b.make_move(*move_found);
                }
            }
            continue;
        } else if (word == "go") {
            if (b.get_legal_moves(b.side_to_play).empty()) {
                std::cerr << "no legal move found to be searched" << std::endl;
                std::cout << "bestmove (none)" << std::endl;
            } else {
                //auto selected_move = e.get_move(b);
                auto selected_move = mcts(b).get_best_move(6);
                std::cout << "bestmove " << to_long_move(selected_move) << std::endl;
            }
        } else if (word == "print") {
            b.print();
        }
    }
    return 0;
}