//
// Created by leon on 2020-01-21.
//

#include <fstream>
#include <random>
#include <cmath>
#include <chess/core.h>
#include <chess/board.h>
#include <chess/game.h>
#include <chess/move_gen.h>
#include <chess/engine/engine.h>
#include <chess/engine/static_evaluator.h>
#include <chess/engine/nn_eval.h>
#include <boost/process.hpp>
#include <chess/uci/engine_wrapper.h>

#define STOCKFISH true

using namespace chess::core;

int main(int argc, char** argv) {

    chess::core::init();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution unif;

    nn_eval nne((chess::neural::mlp(rd(), {nn_eval::INPUT_SIZE, 1})));
    int depth = 2;
#if STOCKFISH
    boost::process::opstream opstr;
    boost::process::ipstream ipstr;
    boost::process::child stockfish("./stockfish10", boost::process::std_in < opstr, boost::process::std_out > ipstr);
    chess::uci::engine_wrapper eng(opstr, ipstr);

    eng.send_uci();
    eng.wait_for_uciok();
    eng.send_isready();
    eng.wait_for_readyok();

    chess::uci::cmd_go go_command;
    go_command.max_depth = depth;
#else
    static_evaluator se;
    engine e(se, depth);
#endif
    transposition_table tt(10'000'000);
    std::ofstream ofs("out.txt");

    tt_node dummy{};

    for (int num_moves = 1; num_moves < 300; num_moves++) {
        for (int k = 0; k < 1000; k++) {
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
            chess::uci::cmd_position position;
            position.initial_position = "startpos";
            for (int m = 1; m < g.states.size(); m++) {
                position.moves.push_back(to_long_move(g.states[m].last_move));
            }
            eng.send_position(position);
            eng.send_go(go_command);
            std::string line;
            int value = 0;
            while (std::getline(ipstr, line, '\n')) {
                std::cout << line << std::endl;
                if (line.substr(0, 8) == "bestmove") break;
                if (line.substr(0, 4) != "info") continue;
                chess::uci::cmd_info info = chess::uci::parse_cmd_info(line);
                if (!info.score_informed) continue;
                if (info.score_mate) {
                    value = info.score > 0 ? 2000 : -2000;
                } else {
                    value = info.score;
                }
            }
#endif
            board bw = b.side_to_play == BLACK ? b.flip_colors() : b;
            nne.fill_input_vector(bw);
#if !STOCKFISH
            int value = depth <= 0 ? se.eval(bw) : e.search_iterate(g).second;
#endif
            double y = double(std::min(std::max(value, -10000), 10000));
            y = y / 10000.0;
            ofs << y << " " << nne.input_vector.transpose() << std::endl;
        }
    }
}