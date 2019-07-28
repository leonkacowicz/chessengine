#ifndef CHESSENGINE_SQUARE_H
#define CHESSENGINE_SQUARE_H

#include <string>
#include <iostream>

class square {
    unsigned char coords;

public:
    square() noexcept : coords(255) {}
    constexpr square(const unsigned int file, const unsigned int rank) noexcept : coords(((rank & 7u) << 4u) | (file & 7u)) {}
    square(const std::string& position) : square(position[0] - 'a', position[1] - '1') {}
    constexpr square(const char* position) : square(position[0] - 'a', position[1] - '1') {}
    constexpr unsigned int get_file() const {
        return coords & 0x7u;
    }

    constexpr unsigned int get_rank() const {
        return (unsigned)(coords >> 4u) & 0x7u;
    }

    bool operator==(square rhs) const {
        return coords == rhs.coords;
    }

    bool operator!=(square rhs) const {
        return coords != rhs.coords;
    }

    constexpr char get_file_char() const {
        return static_cast<char>('a' + get_file());
    }

    constexpr char get_rank_char() const {
        return static_cast<char>('1' + get_rank());
    }

    std::string to_string() const {
        return std::string({get_file_char(), get_rank_char()});
    }

    static const square none;
};

#endif //CHESSENGINE_SQUARE_H
