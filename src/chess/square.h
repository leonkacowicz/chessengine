#ifndef CHESSENGINE_SQUARE_H
#define CHESSENGINE_SQUARE_H

#include <string>
#include <iostream>

class square {
    unsigned char coords;

public:
    square() : coords(255) {}
    constexpr square(const unsigned int x, const unsigned int y) : coords(((y & 7u) << 4u) | (x & 7u)) {}
    square(const std::string& position) : square(position[0] - 'a', position[1] - '1') {}
    square(const char* position) : square(position[0] - 'a', position[1] - '1') {}
    constexpr unsigned int get_file() const {
        return coords & 7u;
    }

    constexpr unsigned int get_rank() const {
        return (unsigned)(coords >> 4u) & 7u;
    }

    bool operator==(square rhs) const {
        return coords == rhs.coords;
    }

    std::string to_string() const {
        char file = static_cast<char>('a' + get_file());
        char rank = static_cast<char>('1' + get_rank());
        return std::string({file, rank});
    }
};



#endif //CHESSENGINE_SQUARE_H
