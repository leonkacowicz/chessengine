//
// Created by leon on 2020-01-31.
//

#include <fstream>
#include <random>
#include <chess/game.h>
#include <chess/move_gen.h>

using namespace chess::core;


void simulate_game(int num_random_moves, std::ostream& out) {
    std::mt19937 mt((std::random_device())());
    std::uniform_int_distribution unif;
    game g;
    std::vector<move> legal = move_gen(g.states.back().b).generate();
    std::vector<std::string> moves;
    moves.reserve(num_random_moves);
    while (true) {
        moves.clear();
        for (int i = 0; i < num_random_moves; i++) {
            if (legal.empty()) break;
            auto m = legal[unif(mt) % legal.size()];
            g.do_move(m);
            moves.push_back(to_long_move(m));
            legal = move_gen(g.states.back().b).generate();
        }
        if (moves.size() == num_random_moves) break;
        else continue;
    }
    out << moves[0];
    for (int i = 1; i < num_random_moves; i++) out << " " << moves[i];
    out << std::endl;
}


int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: gengames [number of games] [num moves per game]" << std::endl;
        std::exit(1);
    }
    int num_games;
    int num_moves;
    try {
        num_games = std::stoi(argv[1]);
        num_moves = std::stoi(argv[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        std::exit(1);
    }
    chess::core::init();
    for (int i = 0; i < num_games; i++) simulate_game(num_moves, std::cout);
}
