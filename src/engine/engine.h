//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>
#include "evaluator.h"

class engine {

    const board& b;
    evaluator eval;
public:
    engine(const board& b): b(b) {}
    move get_move();

    int minimax(const board& brd, int ply, std::vector<move>& legal_moves, std::vector<move>& sequence, bool log, int alpha, int beta);
};


#endif //CHESSENGINE_ENGINE_H
