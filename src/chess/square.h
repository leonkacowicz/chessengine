#ifndef CHESSENGINE_SQUARE_H
#define CHESSENGINE_SQUARE_H

#include <string>
#include <iostream>

namespace chess::core {
    enum square : int {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        SQ_NONE
    };

    constexpr int get_rank(square sq) {
        return sq / 8;
    }

    constexpr int get_file(square sq) {
        return sq % 8;
    }

    constexpr square get_square(int file, int rank) {
        return static_cast<enum square>(file + 8 * rank);
    }

    constexpr square get_square(const char* s) {
        return get_square(s[0] - 'a', s[1] - '1');
    }

    constexpr char get_file_char(square sq) {
        return char('a' + get_file(sq));
    }

    constexpr char get_rank_char(square sq) {
        return char('1' + get_rank(sq));
    }

    inline std::ostream& operator<<(std::ostream& os, square sq) {
        return os << get_file_char(sq) << get_rank_char(sq);
    }

    constexpr int file_dist(square sq1, square sq2) {
        int f = get_file(sq1) - get_file(sq2);
        f = f < 0 ? -f : f;
        return f;
    }

    constexpr int rank_dist(square sq1, square sq2) {
        int r = get_rank(sq1) - get_rank(sq2);
        r = r < 0 ? -r : r;
        return r;
    }

    constexpr square operator+(square sq, int i) {
        return square(int(sq) + i);
    }

    constexpr square& operator+=(square& sq, int i) {
        return sq = sq + i;
    }

    constexpr square& operator++(square& sq) {
        return sq = sq + 1;
    }
}

#endif //CHESSENGINE_SQUARE_H
