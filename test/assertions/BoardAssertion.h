//
// Created by leon on 19/06/18.
//

#ifndef CHESSENGINE_BOARDASSERTION_H
#define CHESSENGINE_BOARDASSERTION_H


#include "../../src/engine/Board.h"
#include "Assertion.h"

template <>
class Assertion<Board> {
private:
    Board const & board;
public:
    Assertion(Board const & board) : board(board) {};
    void isNotCheckmate() const;
};


#endif //CHESSENGINE_BOARDASSERTION_H
