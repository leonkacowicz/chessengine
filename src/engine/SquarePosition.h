#ifndef CHESSENGINE_SQUAREPOSITION_H
#define CHESSENGINE_SQUAREPOSITION_H

#include <string>
#include <iostream>

using namespace std;

class SquarePosition {
    unsigned char position;

public:
    SquarePosition() : position(0) {}
    constexpr SquarePosition(const unsigned int x, const unsigned int y) : position(((y & 7u) << 4u) | (x & 7u)) {}
    SquarePosition(const string & position) : SquarePosition(position[0] - 'a', position[1] - '1') {}
    constexpr unsigned int getX() const {
        return position & 7u;
    }

    constexpr unsigned int getY() const {
        return (unsigned)(position >> 4u) & 7u;
    }

    bool operator==(SquarePosition rhs) const {
        return position == rhs.position;
    }
};



#endif //CHESSENGINE_SQUAREPOSITION_H
