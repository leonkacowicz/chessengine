#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <chess/core.h>
#include <chess/move_gen.h>
#include <chess/game.h>

using std::stringstream;
using std::string;
using namespace chess::core;

std::vector<string> split(const string& input, const string& delimiter = " ") {
    auto start = 0U;
    auto end = input.find(delimiter);
    auto delim_len = delimiter.length();
    std::vector<string> ret;
    while (end != std::string::npos) {
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

game handle_position_cmd(const std::vector<string>& words) {
    assert(words[0] == "position");
    if (words[1] == "startpos") {
        board b;
        b.set_initial_position();
        game g(b);
        auto iter = std::find(words.begin(), words.end(), "moves");
        if (iter == words.end()) return g;
        while (++iter != words.end()) {
            auto moves = move_gen(g.states.back().b).generate();
            auto move_found = std::find_if(moves.begin(), moves.end(), [&] (const move& m) {
                return to_long_move(m) == *iter;
            });
            if (move_found != moves.end()) {
                g.do_move(*move_found);
            } else {
                print_illegal_start_sequence_message(words, iter - words.begin() + 1);
                return g;
            }
        }
        return g;
    }
    throw std::runtime_error("position type not implemented");
}

int main()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif(0);
    chess::core::init();
    board b;
    b.set_initial_position();
    game g(b);
    string input;
    while (std::getline(std::cin, input)) {
        auto words = split(input);

        if (words[0] == "uci") {
            std::cout << "uciok" << std::endl;
            std::cout.flush();
            continue;
        } else if (words[0] == "isready") {
            std::cout << "readyok" << std::endl;
            std::cout.flush();
            continue;
        } else if (words[0] == "position") {
            if (words[1] == "startpos") {
                g = handle_position_cmd(words);
            }
            continue;
        } else if (words[0] == "go") {
            auto legal_moves = move_gen(g.states.back().b).generate();
            if (legal_moves.empty()) {
                std::cout << "bestmove (none)" << std::endl;
            } else {
                int size = legal_moves.size();
                int random_move_index = unif(mt) % size;
                std::cout << "bestmove " << to_long_move(legal_moves[random_move_index]) << std::endl;
                std::cout.flush();
            }
        } else if (words[0] == "print") {
            b.print();
        }
    }
    return 0;
}