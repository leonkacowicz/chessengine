#include <iostream>
#include <string>
#include <algorithm>
#include <board.h>
#include "engine.h"

using std::stringstream;
using std::string;

int main()
{
    board b;
    b.set_initial_position();
    string line;
    while (!std::cin.eof()) {
        std::getline(std::cin, line);
        std::cerr << "RECEIVED: " << line << std::endl;

        stringstream ss;
        ss << line;
        string word;
        ss >> word;
        std::cerr << "Processing command: " << word << std::endl;
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
                    return m.to_long_move() == word;
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
            std::cerr << "Calculating..." << std::endl;
            engine e(b);
            for (auto m : b.get_legal_moves(b.side_to_play)) {
                std::cerr << m.to_long_move() << std::endl;
            }
            auto selected_move = e.get_move();
            std::cout << "bestmove " << selected_move.to_long_move() << std::endl;
        } else if (word == "print") {
            b.print();
            std::cerr << (b.side_to_play == WHITE ? "White " : "Black ") << " to play" << std::endl;
        }
    }
    return 0;
}