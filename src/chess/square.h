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
    constexpr char get_file() const {
        return (char)(coords & 0x7u);
    }

    constexpr char get_rank() const {
        return (char)(((unsigned)coords >> 4u) & 0x7u);
    }

    constexpr bool operator==(square rhs) const {
        return coords == rhs.coords;
    }

    constexpr bool operator!=(square rhs) const {
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

    constexpr bool is_none() const {
        return coords == 255;
    }

    const static square none;
};

#endif //CHESSENGINE_SQUARE_H
