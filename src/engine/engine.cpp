//
// Created by leon on 7/28/19.
//

#include <deque>
#include "engine.h"
#include "evaluator.h"

int minimax(const board& brd, int ply, std::vector<move>& sequence, move try_first, int alpha, int beta) {
    if (ply == 0) {
        return evaluator().evaluate(brd);
    } else {
        const std::vector<move> legal_moves = brd.get_legal_moves(brd.side_to_play);
        std::deque<move> legal(legal_moves.begin(), legal_moves.end(), legal_moves.get_allocator());
        if (try_first.special != NULL_MOVE) {
            legal.push_front(try_first);
        }
        int k = 0;
        for (auto& m : legal) {
            k++;
            if (k > 1 && m == try_first) continue;
            board bnew = brd;
            bnew.make_move(m);
            int value = -minimax(bnew, ply - 1, sequence, move(), -beta, -alpha);
            if (value >= beta) {
                return beta;
            }
            if (value > alpha) {
                //std::cout << "info depth " << ply << " cp " << value << std::endl;
                alpha = value;
                sequence[ply] = m;
            }
        }
        return alpha;
    }
}

move engine::get_move() const {
    int plys = 6;
    std::vector<move> seq(plys + 1);
    for (int ply = 1; ply <= plys; ply++) {
        std::cerr << "Trying starting with move " << seq[ply - 1].to_long_move() << std::endl;
        int value = minimax(b, ply, seq, seq[ply - 1], -1000000, 1000000);
        std::cout << "info depth " << ply << " cp " << value << std::endl;
        std::cerr << "Current best move " << seq[ply].to_long_move() << std::endl;
    }
    return seq[plys];
}
