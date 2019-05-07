#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H


#include "SquarePosition.h"

class Move {
private:
    SquarePosition origin;
    SquarePosition destination;

public:
    Move(SquarePosition origin, SquarePosition destination) : origin(origin), destination(destination) {};
};


#endif //CHESSENGINE_MOVE_H
