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

class engine {
public:
    engine();
    move search_iterate(const board& b);
};


#endif //CHESSENGINE_ENGINE_H
