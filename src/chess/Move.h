#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H


#include "Square.h"

class Move {
private:
    Square origin;
    Square destination;
    char promotion; // 0, n, r, b, q

public:
    Move(Square origin, Square destination) : origin(origin), destination(destination), promotion(0) {};
    Move(Square origin, Square destination, char promotion) : origin(origin), destination(destination), promotion(promotion) {};

    bool operator==(const Move& rhs) const {
        return destination == rhs.destination && origin == rhs.origin && promotion == rhs.promotion;
    }
};


#endif //CHESSENGINE_MOVE_H
