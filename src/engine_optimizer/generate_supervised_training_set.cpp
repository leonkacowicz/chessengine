//
// Created by leon on 2020-01-21.
//

#include <fstream>
#include <random>
#include <chess/core.h>
#include <chess/board.h>
#include <chess/game.h>
#include <chess/move_gen.h>
#include <chess/engine/engine.h>
#include <chess/engine/static_evaluator.h>
#include <chess/engine/nn_eval.h>
#include <boost/process.hpp>

#define STOCKFISH true

using namespace chess::core;

int main(int argc, char** argv) {

    init();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif;

    nn_eval nne((chess::neural::mlp(rd, {nn_eval::INPUT_SIZE, 1})));
    int depth = 0;
#if STOCKFISH
    boost::process::opstream opstr;
    boost::process::ipstream ipstr;
    boost::process::child stockfish("./stockfish10", boost::process::std_in < opstr, boost::process::std_out > ipstr);
    opstr << "uci" << std::endl;
    opstr << "isready" << std::endl;
    opstr.flush();
#else
    static_evaluator se;
    engine e(se, depth);
#endif
    transposition_table tt(10'000'000);
    std::ofstream ofs("out.txt");

    tt_node dummy{};

    for (int num_moves = 1; num_moves < 400; num_moves++) {
        for (int k = 0; k < 30; k++) {
            game g;

            bool terminal = false;
            std::vector<std::vector<move>> moves_lists;
            for (int m = 0; m < num_moves; m++) {
                auto legal = move_gen(g.states.back().b).generate();
                if (legal.empty()) {
                    terminal = true;
                    break;
                }
                g.do_move(legal[unif(mt) % legal.size()]);
                moves_lists.push_back(legal);
            }
            if (terminal) {
                k--;
                continue;
            }
            if (tt.load(g.states.back().hash, depth, &dummy)) continue;

            tt.save(g.states.back().hash, depth, 0, EXACT, NULL_MOVE);
            std::printf("k = %d, move = %d\n", k, int(g.states.size()));
            const auto& b = g.states.back().b;
            b.print();
#if STOCKFISH
            std::cout << "ucinewgame" << std::endl;
            std::cout.flush();
            opstr << "ucinewgame" << std::endl;
            opstr.flush();
            std::cout << "position startpos moves";
            opstr << "position startpos moves";
            for (int m = 1; m < g.states.size(); m++) {
                std::cout << " " << to_long_move(g.states[m].last_move);
                opstr << " " << to_long_move(g.states[m].last_move);
            }
            std::cout << std::endl << "go depth " << depth << std::endl;
            opstr << std::endl;
            opstr.flush();
            opstr << "go depth " << depth << std::endl;
            std::cout.flush();
            opstr.flush();
            std::string line;
            int value = 0;
            while (std::getline(ipstr, line, '\n')) {
                std::cout << line << std::endl;
                std::stringstream ss(line);
                std::string word;
                ss >> word;
                if (word == "bestmove") break;
                if (word != "info") continue;
                ss >> word; //depth
                int current_depth;
                ss >> current_depth;
                if (current_depth < depth) continue;
                ss >> word; //seldepth
                ss >> word; //seldepth
                ss >> word; //multipv
                ss >> word; //multipv
                ss >> word; //score
                ss >> word; //score type
                int score;
                ss >> score;
                if (word == "cp") {
                    value = score;
                } else {
                    value = score > 0 ? 2000 : -2000;
                }
            }
            if (b.side_to_play == BLACK) value = -value;
#endif

            nne.fill_input_vector(b.side_to_play == BLACK ? b.flip_colors() : b);
#if !STOCKFISH
            int value = depth <= 0 ? se.eval(b) : e.search_iterate(g).second;
#endif
            double y = double(std::min(std::max(value, -10000), 10000));
            y = y / 10000.0;
            ofs << (b.side_to_play == BLACK ? -y : y) << " " << nne.input_vector.transpose() << std::endl;
            nne.fill_input_vector(b.side_to_play == WHITE ? b.flip_colors() : b);
            ofs << (b.side_to_play == WHITE ? -y : y) << " " << nne.input_vector.transpose() << std::endl;
        }
    }
}