//
// Created by leon on 2020-01-21.
//

#include <fstream>
#include <random>
#include <core.h>
#include <board.h>
#include <game.h>
#include <move_gen.h>
#include <engine.h>
#include <static_evaluator.h>
#include <nn_eval.h>

using namespace chess::core;

int main(int argc, char** argv) {

    init();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif;

    nn_eval nne((chess::neural::mlp(rd, {INPUT_SIZE, 1})));
    static_evaluator se;
    engine e(se, 4);
    std::ofstream ofs("out.txt");

    for (int k = 0; k < 100; k++) {

        chess::core::board b;
        b.set_initial_position();
        chess::core::game g(b);
        
        for (int i = 0; i < 200; i++) {
            std::printf("k = %d; i = %d\n", k, i);
            auto board = g.states.back().b;
            auto legal = move_gen(board).generate();
            if (legal.empty()) break;
            move m = legal[unif(mt) % legal.size()];
            g.do_move(m);
            board = g.states.back().b;
            board.print();
            auto pair = e.search_iterate(g);
            nne.fill_input_vector(i % 2 ? board.flip_colors() : board);
            double y = double(std::min(std::max(pair.second, -10000), 10000));
            y = y / (std::abs(y) + 1000.0);
            ofs << y << " " << nne.input_vector.transpose() << std::endl;
        }
    }

}