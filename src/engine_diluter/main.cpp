#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <chess/core.h>
#include <chess/move_gen.h>
#include <chess/game.h>
#include <boost/process.hpp>
#include <boost/program_options.hpp>

using std::stringstream;
using std::string;
using namespace chess::core;

struct cmdline_options {
    double engine_move_probability;
    uint64_t seed;
    bool seed_provided;
    std::string engine_cmd_line;
};

cmdline_options parse_argv(int argc, char** argv) {
    cmdline_options o{};
    o.engine_move_probability = 0.0;
    try {
        boost::program_options::options_description options;
        options.add_options()
                ("help,h", "pring usage message")
                ("prob,p", boost::program_options::value<double>()->required(), "engine move probability (1 equivalent of engine playing directly, 0 equivalent of random-engine playing directly)")
                ("seed,s", boost::program_options::value<uint64_t>(), "random seed number")
                ("exec,e", boost::program_options::value<std::string>()->required(), "underlying engine exec cmd line")
                ;

        boost::program_options::variables_map vars;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vars);

        if (vars.count("help")) {
            std::cout << options << std::endl;
            std::exit(0);
        }
        if (vars.count("seed")) {
            o.seed_provided = true;
            o.seed = vars["seed"].as<uint64_t>();
        }
        if (vars.count("prob")) {
            o.engine_move_probability = vars["prob"].as<double>();
            if (o.engine_move_probability < 0 || o.engine_move_probability > 1) {
                std::cerr << "Invalid probability provided: " << o.engine_move_probability << std::endl;
                std::cerr << "Probability must be >= 0 and <= 1" << std::endl;
                std::exit(1);
            }
        }
        o.engine_cmd_line = vars["exec"].as<std::string>();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    return o;
}

std::vector<string> split(const string& input, char delimiter = ' ') {
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> ret;
    while (std::getline(ss, token, delimiter))
        if (!token.empty()) ret.push_back(token);
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
    cmdline_options options = parse_argv(argc, argv);
    boost::process::ipstream ipstream;
    boost::process::opstream opstream;
    boost::process::child child(options.engine_cmd_line, boost::process::std_in < opstream);

    if (!options.seed_provided) {
        std::random_device rd;
        options.seed = rd();
        std::cout << "info string using random seed " << options.seed << std::endl;
    } else {
        std::cout << "info string using provided random seed " << options.seed << std::endl;
    }
    std::mt19937 mt(options.seed);
    std::uniform_int_distribution unif(0);
    std::uniform_real_distribution unif01(0., 1.);
    chess::core::init();
    game g;
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
            if (unif01(mt) < options.engine_move_probability) {
                opstream << input << std::endl;
            } else {
                std::cout << "info string playing a random move this turn" << std::endl;
                auto legal_moves = move_gen(g.states.back().b).generate();
                if (legal_moves.empty()) {
                    std::cout << "bestmove (none)" << std::endl;
                } else {
                    int size = legal_moves.size();
                    int random_move_index = unif(mt) % size;
                    std::cout << "bestmove " << to_long_move(legal_moves[random_move_index]) << std::endl;
                }
            }
        } else if (words[0] == "print") {
            g.states.back().b.print();
        } else {
            opstream << input << std::endl;
        }
    }
    return 0;
}