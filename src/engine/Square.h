#ifndef CHESSENGINE_SQUARE_H
#define CHESSENGINE_SQUARE_H

#include <string>
#include <iostream>

using namespace std;

class Square {
    unsigned char coords;

public:
    Square() : coords(0) {}
    constexpr Square(const unsigned int x, const unsigned int y) : coords(((y & 7u) << 4u) | (x & 7u)) {}
    Square(const string & position) : Square(position[0] - 'a', position[1] - '1') {}
    constexpr unsigned int getX() const {
        return coords & 7u;
    }

    constexpr unsigned int getY() const {
        return (unsigned)(coords >> 4u) & 7u;
    }

    bool operator==(Square rhs) const {
        return coords == rhs.coords;
    }
};



#endif //CHESSENGINE_SQUARE_H
