//
// Created by leon on 2020-01-31.
//

#include <fstream>
#include <random>
#include <filesystem>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <chess/game.h>
#include <chess/move_gen.h>
#include <chess/neuralnet/mlp.h>
#include <chess/engine/nn_eval.h>
#include <chess/engine/engine.h>

using namespace chess::core;

void initialize_weights() {
    if (std::filesystem::exists("weights.txt")) return;
    std::random_device rd;
    chess::neural::mlp mlp(rd(), {nn_eval::INPUT_SIZE, 100, 1});
    mlp.output_to_stream(std::ofstream("weights.txt"));
}

void initialize_training_set_file() {
    std::ofstream ofs("train.csv");
    ofs << "y";
    for (int i = 0; i < nn_eval::INPUT_SIZE; i++) ofs << ",attr" << i;
    ofs << std::endl;
}

void add_training_sample(const board& b, int value, nn_eval& eval) {
    std::ofstream ofs("train.csv", std::ofstream::out | std::ofstream::app);
    board board_ = b.side_to_play == BLACK ? b.flip_colors() : b;
    eval.fill_input_vector(board_);
    double y = double(std::min(std::max(value, -10000), 10000)) / 10000.0;
    ofs << y;
    for (int i = 0; i < eval.input_vector.size(); i++) ofs << "," << eval.input_vector[i];
    ofs << std::endl;

    eval.fill_input_vector(board_.flip_colors());
    ofs << -y;
    for (int i = 0; i < eval.input_vector.size(); i++) ofs << "," << eval.input_vector[i];
    ofs << std::endl;
}

void simulate_game(const chess::neural::mlp& net, int num_random_moves) {
    std::mt19937 mt((std::random_device())());
    std::uniform_int_distribution unif;
    nn_eval eval(net);
    engine engs[] = {engine(eval, 8), engine(eval, 8)};

    game g;
    std::vector<move> legal(0);
    legal = move_gen(g.states.back().b).generate();
    for (int i = 0; i < num_random_moves; i++) {
        if (legal.empty()) return;
        g.states.back().b.print();
        g.do_move(legal[unif(mt) % legal.size()]);
        legal = move_gen(g.states.back().b).generate();
    }

    while (!legal.empty() && !g.is_draw()) {
        board b = g.states.back().b;
        std::cout << (b.side_to_play ? "\n\nblack turn\n": "\n\nwhite turn\n");
        b.print();
        std::pair<move, int> pair = engs[b.side_to_play].search_iterate(g);
        add_training_sample(b, pair.second, eval);
        g.do_move(pair.first);
        legal = move_gen(g.states.back().b).generate();
    }
}

void simulate_games(int num_games) {
    std::ifstream ifs("weights.txt");
    chess::neural::mlp net(ifs);
    for (int i = 0; i < num_games; i++) {
        simulate_game(net, 8);
    }
}

void run_supervised_trainer() {

}

int main() {
// 1. initialize weights
// 2. simulate N games against itself
// 3. from game results, create supervised training set with each position -> minimax assessed result
// 4. run supervised training starting from previous weights, with newly generated training set
// 5. go to step 2

    chess::core::init();
    initialize_weights();
    initialize_training_set_file();
    simulate_games(20);
    run_supervised_trainer();


}
