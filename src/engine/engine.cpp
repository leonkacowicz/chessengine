//
// Created by leon on 7/28/19.
//

#include "engine.h"
#include "evaluator.h"

int minimax(const board& brd, int ply, std::vector<move>& sequence, int alpha, int beta) {
    if (ply == 0) {
        return evaluator().evaluate(brd);
    } else {
        const std::vector<move> legal_moves = brd.get_legal_moves(brd.side_to_play);

        int max = -100000;
        move best;
        for (auto& m : legal_moves) {
            board bnew = brd;
            bnew.make_move(m);
            int value = -minimax(bnew, ply - 1, sequence, -beta, -alpha);
            if (value >= beta) {
                return beta;
            }
            if (value > alpha) {
                std::cout << "info depth " << ply << " cp " << value << std::endl;
                alpha = value;
                best = m;
            }
        }
        sequence[ply] = best;
        return alpha;
    }
}

move engine::get_move() const {
    int plys = 5;
    std::vector<move> seq(plys + 1);
    int value = minimax(b, plys, seq, -100000, 100000);
    return seq[plys];
}
