#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H

#include <string>
#include <sstream>
#include "square.h"

enum special_move {
    CASTLE_KING_SIDE_WHITE = 1,
    CASTLE_QUEEN_SIDE_WHITE,
    CASTLE_KING_SIDE_BLACK,
    CASTLE_QUEEN_SIDE_BLACK,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT
};

class move {
private:

public:
    square origin;
    square destination;
    char special; // 0, n, r, b, q
    move(square origin, square destination) : origin(origin), destination(destination), special(0) {};
    move(square origin, square destination, special_move special) : origin(origin), destination(destination), special(special) {};

    bool operator==(const move& rhs) const {
        return destination == rhs.destination && origin == rhs.origin && special == rhs.special;
    }

    std::string to_long_move() const {
        std::stringstream ss;
        ss << origin.to_string() << destination.to_string();
        if (special == PROMOTION_QUEEN) ss << 'q';
        if (special == PROMOTION_ROOK) ss << 'r';
        if (special == PROMOTION_BISHOP) ss << 'b';
        if (special == PROMOTION_KNIGHT) ss << 'n';
        std::string s;
        ss >> s;
        return s;
    }
};


#endif //CHESSENGINE_MOVE_H
