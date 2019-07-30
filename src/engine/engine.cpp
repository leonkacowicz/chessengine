//
// Created by leon on 7/28/19.
//

#include <deque>
#include <algorithm>
#include "engine.h"
#include "evaluator.h"

int engine::minimax(const board& brd, int ply, std::vector<move>& legal_moves, std::vector<move>& sequence, bool log, int alpha, int beta) {
    if (ply == 0) {
        return eval.evaluate(brd);
    } else {
        if (brd.side_to_play == WHITE) {
            int best = alpha;
            for (auto& m : legal_moves) {
                board bnew = brd;
                bnew.make_move(m);
                auto moves = bnew.get_legal_moves(bnew.side_to_play);
                int val = minimax(bnew, ply - 1, moves, sequence, false, alpha, beta);

                if (val >= beta) return beta;
                if (val > best) {
                    best = val;
                    sequence[ply] = m;
                    if (log) {
                        std::cout << "info depth " << ply << " score cp " << val << " " << m.to_long_move() << std::endl;
                    }
                }
                if (best > alpha) alpha = best;
            }
            return best;
        } else {
            int best = beta;
            for (auto& m : legal_moves) {
                board bnew = brd;
                bnew.make_move(m);
                auto moves = bnew.get_legal_moves(bnew.side_to_play);
                int val = minimax(bnew, ply - 1, moves, sequence, false, alpha, beta);

                if (val <= alpha) return alpha;
                if (val < best) {
                    best = val;
                    sequence[ply] = m;
                    if (log) {
                        std::cout << "info depth " << ply << " cp " << val << " " << m.to_long_move() << std::endl;
                    }
                }
                if (best < beta) beta = best;
            }
            return best;
        }
    }
}

move engine::get_move() {
    int plys = 6;
    std::vector<move> legal_moves = b.get_legal_moves(b.side_to_play);
    if (legal_moves.empty()) return {};
    std::vector<move> seq(plys + 1);

    for (int ply = 1; ply <= plys; ply++) {
        std::cerr << "Trying starting with move " << seq[ply - 1].to_long_move() << std::endl;
        int value = minimax(b, ply, legal_moves, seq, true, -1'000'000'000, 1'000'000'000);

        std::cout << "info currmove "; for (move m : legal_moves) std::cout << " " << m.to_long_move(); std::cout << std::endl;
        legal_moves.erase(std::find_if(legal_moves.begin(), legal_moves.end(), [&] (const move m) { return m == seq[ply]; }));
        legal_moves.insert(legal_moves.begin(), seq[ply]);
        std::cout << "info currmove "; for (move m : legal_moves) std::cout << " " << m.to_long_move(); std::cout << std::endl;

        std::cerr << "Current best move " << seq[ply].to_long_move() << std::endl;
    }
    if (seq[plys].special == NULL_MOVE) {
        return legal_moves[0];
    }
    return seq[plys];
}
