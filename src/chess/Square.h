#ifndef CHESSENGINE_SQUARE_H
#define CHESSENGINE_SQUARE_H

#include <string>
#include <iostream>

class Square {
    unsigned char coords;

public:
    Square() : coords(0) {}
    constexpr Square(const unsigned int x, const unsigned int y) : coords(((y & 7u) << 4u) | (x & 7u)) {}
    Square(const std::string & position) : Square(position[0] - 'a', position[1] - '1') {}
    constexpr unsigned int getX() const {
        return coords & 7u;
    }

    constexpr unsigned int getY() const {
        return (unsigned)(coords >> 4u) & 7u;
    }

    bool operator==(Square rhs) const {
        return coords == rhs.coords;
    }

    std::string to_string() const {
        char file = static_cast<char>('a' + getX());
        char rank = static_cast<char>('1' + getY());
        return std::string({file, rank});
    }
};



#endif //CHESSENGINE_SQUARE_H
