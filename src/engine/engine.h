//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>
#include <unordered_map>
#include "evaluator.h"
#include "zobrist.h"

struct node {
    int val;
    move m;
    bool beta_cut;
};


class engine {
public:
    engine();
    move get_move(const board& b);
};


#endif //CHESSENGINE_ENGINE_H
