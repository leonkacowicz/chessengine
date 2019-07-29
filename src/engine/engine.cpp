//
// Created by leon on 7/28/19.
//

#include <deque>
#include "engine.h"
#include "evaluator.h"

int minimax(const board& brd, int ply, std::vector<move>& sequence, bool log, int alpha, int beta) {
    if (ply == 0) {
        return 0; //evaluator().evaluate(brd);
    } else {
        const std::vector<move> legal_moves = brd.get_legal_moves(brd.side_to_play);
        if (brd.side_to_play == WHITE) {
            int best = -1'000'000;
            for (auto& m : brd.get_legal_moves(WHITE)) {
                board bnew = brd;
                bnew.make_move(m);
                int val = minimax(bnew, ply - 1, sequence, false, alpha, beta);

                if (val >= best) {
                    best = val;
                    sequence[ply] = m;
                    if (log) {
                        std::cout << "info depth " << ply << " cp " << val << " " << m.to_long_move() << std::endl;
                    }
                }
                if (best > alpha) alpha = best;
                if (beta <= alpha) break;
            }
            return best;
        } else {
            int best = 1'000'000;
            for (auto& m : brd.get_legal_moves(BLACK)) {
                board bnew = brd;
                bnew.make_move(m);
                int val = minimax(bnew, ply - 1, sequence, false, alpha, beta);

                if (val <= best) {
                    best = val;
                    sequence[ply] = m;
                    if (log) {
                        std::cout << "info depth " << ply << " cp " << val << " " << m.to_long_move() << std::endl;
                    }
                }
                if (best < beta) beta = best;
                if (beta <= alpha) break;
            }
            return best;
        }
    }
}

move engine::get_move() const {
    int plys = 6;
    std::vector<move> seq(plys + 1);
    for (int ply = 1; ply <= plys; ply++) {
        std::cerr << "Trying starting with move " << seq[ply - 1].to_long_move() << std::endl;
        int value = minimax(b, ply, seq, true, -1000000, 1000000);
        std::cout << "info depth " << ply << " cp " << value << std::endl;
        std::cerr << "Current best move " << seq[ply].to_long_move() << std::endl;
    }
    return seq[plys];
}
