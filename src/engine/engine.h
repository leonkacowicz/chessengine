//
// Created by leon on 7/28/19.
//

#ifndef CHESSENGINE_ENGINE_H
#define CHESSENGINE_ENGINE_H


#include <move.h>
#include <board.h>

class engine {

    const board& b;
public:
    engine(const board& b): b(b) {}
    move get_move() const;
};


#endif //CHESSENGINE_ENGINE_H
