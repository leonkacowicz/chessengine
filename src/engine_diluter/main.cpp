#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <chess/core.h>
#include <chess/move_gen.h>
#include <chess/game.h>
#include <boost/process.hpp>

using std::stringstream;
using std::string;
using namespace chess::core;

struct cmdline_options {
    double random_move_prob;
    std::vector<std::string> engine_cmd_line;
};

cmdline_options parse_argv(int argc, char** argv) {
    cmdline_options o{};
    o.random_move_prob = 0.0;
    bool arg_is_prob = false;
    for (int i = 1; i < argc; i++) {
        if (arg_is_prob) {
            o.random_move_prob = std::stod(argv[i]);
            arg_is_prob = false;
        }
        else if (std::string(argv[i]) == "-p" ||  std::string(argv[i]) == "--prob") arg_is_prob = true;
        else o.engine_cmd_line.emplace_back(argv[i]);
    }
    if (o.random_move_prob < 0 || o.random_move_prob > 1) {
        std::cerr << "Invalid probability provided: " << o.random_move_prob << std::endl;
        std::cerr << "Probability must be >= 0 and <= 1" << std::endl;
        std::exit(1);
    }
    return o;
}

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

int main(int argc, char** argv) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif(0);
    std::uniform_real_distribution unif01(0., 1.);
    const cmdline_options options = parse_argv(argc, argv);
    boost::process::ipstream ipstream;
    boost::process::opstream opstream;
    boost::process::child child(options.engine_cmd_line, boost::process::std_in < opstream);
    chess::core::init();
    board b;
    b.set_initial_position();
    game g(b);
    string input;
    while (std::getline(std::cin, input)) {
        auto words = split(input);
        if (words[0] == "position") {
            opstream << input << std::endl;
            if (words[1] == "startpos") {
                g = handle_position_cmd(words);
            }
            continue;
        } else if (words[0] == "go") {
            if (unif01(mt) < options.random_move_prob) {
                std::cout << "info string playing a random move this turn" << std::endl;
                auto legal_moves = move_gen(g.states.back().b).generate();
                if (legal_moves.empty()) {
                    std::cout << "bestmove (none)" << std::endl;
                } else {
                    int size = legal_moves.size();
                    int random_move_index = unif(mt) % size;
                    std::cout << "bestmove " << to_long_move(legal_moves[random_move_index]) << std::endl;
                    std::cout.flush();
                }
            } else {
                opstream << input << std::endl;
            }
        } else if (words[0] == "print") {
            b.print();
        } else {
            opstream << input << std::endl;
        }
    }
    return 0;
}