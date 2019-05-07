#ifndef CHESSENGINE_SQUAREPOSITION_H
#define CHESSENGINE_SQUAREPOSITION_H

#include <string>

class SquarePosition {
    unsigned char position;

public:
    SquarePosition() : position(0) {}
    constexpr SquarePosition(const unsigned int x, const unsigned int y) : position(((y & 0xFu) << 4u) | (x & 0xFu)) {}
    SquarePosition(const std::string & position) : SquarePosition(position[0] - 'a', position[1] - '1') {}
    unsigned int getX() const {
        return position & 0x7u;
    }

    unsigned int getY() const {
        return (unsigned)(position >> 4u) & 0x7u;
    }

    bool operator==(SquarePosition rhs) const {
        return position == rhs.position;
    }
};

#endif //CHESSENGINE_SQUAREPOSITION_H
