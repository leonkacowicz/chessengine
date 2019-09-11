#include <iostream>
#include <string>
#include <algorithm>
#include <board.h>
#include <magic_bitboard.h>
#include "engine.h"
#include "uci.h"

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

void print_illegal_start_sequence_message(const std::vector<string>& words,
        const std::vector<string>::iterator& iter_pos) {

    std::cerr << "ERROR: Illegal move sequence from startpos:";
    for (auto s = words.begin(); s != iter_pos; s++) {
        std::cerr << " " << *s;
    }
    std::cerr << " " << *iter_pos << std::endl;
}

int main()
{
    chess::core::init_bitboards();
    chess::core::init_magic_bitboards();
    zobrist::init();

    engine e;
    board b;
    b.set_initial_position();
    while (!std::cin.eof()) {
        string input;
        std::getline(std::cin, input);
        auto words = split(input);

        if (words[0] == "uci") {
            std::cout << "uciok" << std::endl;
            continue;
        } else if (words[0] == "position") {
            if (words[1] == "startpos") {
                b.set_initial_position();
                auto iter = std::find(words.begin() + 2, words.end(), "moves");
                while (++iter != words.end()) {
                    auto moves = b.get_legal_moves(b.side_to_play);
                    auto move_found = std::find_if(moves.begin(), moves.end(), [&] (const move& m) {
                        return to_long_move(m) == *iter;
                    });
                    if (move_found != moves.end()) {
                        b.make_move(*move_found);
                    } else {
                        print_illegal_start_sequence_message(words, iter);
                        break;
                    }
                }
            }
            continue;
        } else if (words[0] == "go") {
            uci_go_cmd cmd(words);
            if (b.get_legal_moves(b.side_to_play).empty()) {
                std::cerr << "no legal move found to be searched" << std::endl;
                std::cout << "bestmove (none)" << std::endl;
            } else {
                auto selected_move = e.search_iterate(b);
                std::cout << "bestmove " << to_long_move(selected_move) << std::endl;
            }
        } else if (words[0] == "print") {
            b.print();
        }
    }
    return 0;
}