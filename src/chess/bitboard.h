#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cassert>
#include <cstdint>
#include <ostream>
#include <initializer_list>
#include <sstream>
#include "square.h"
#include "core.h"

namespace chess::core {
typedef uint64_t bitboard;
enum shift_direction : int {
    UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1,
    UP_LEFT =
    UP + LEFT, // not necessary but leaving them there as example: one could call shift<3 * UP + 2 * LEFT>(b)
    UP_RIGHT = UP + RIGHT,
    DOWN_LEFT = DOWN + LEFT,
    DOWN_RIGHT = DOWN + RIGHT,
};

constexpr shift_direction operator+(shift_direction a, shift_direction b) {
    return static_cast<shift_direction>(int64_t(a) + int64_t(b));
}

constexpr shift_direction operator*(int a, shift_direction b) {
    return static_cast<shift_direction>(int64_t(a) * int64_t(b));
}

// stupid functions to supress shift warnings
constexpr bitboard shift_left(bitboard arg, uint64_t sh) {
    return arg << sh;
}

constexpr bitboard shift_right(bitboard arg, uint64_t sh) {
    return arg >> sh;
}

template<shift_direction d>
inline bitboard shift(bitboard arg) {
    if (d > 0) return shift_left(arg, d);
    else if (d < 0) return shift_right(arg, -d);
    else return arg;
}

inline square msb(bitboard b) {
    assert(b);
    return square(63 ^ __builtin_clzll(b));
}

inline square lsb(bitboard b) {
    assert(b);
    return square(__builtin_ctzll(b));
}

/**
 * Return the number of squares marked by a bitboard
 * @param bitboard b
 * @return int representing the number of squares
 */
inline int num_squares(bitboard b) {
    // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    return __builtin_popcountl(b);
}

/**
 * Making explicit that we are changing the argument by receiving argument as pointer
 * @param bitboard *b, will be altered to clear its least significan bit
 * @return the position of the least significant bit
 */
inline square pop_lsb(bitboard* b) {
    const square s = lsb(*b);
    *b &= *b - 1;
    return s;
}

extern bitboard piece_attacks_bb[6][64];
extern bitboard pawn_attacks_bb[2][64];
extern bitboard line[64][64];
extern bitboard line_segment[64][64];

void init_bitboards();

constexpr bitboard file_a = 0x0101010101010101;
constexpr bitboard file_b = 0x0202020202020202;
constexpr bitboard file_c = 0x0404040404040404;
constexpr bitboard file_d = 0x0808080808080808;
constexpr bitboard file_e = 0x1010101010101010;
constexpr bitboard file_f = 0x2020202020202020;
constexpr bitboard file_g = 0x4040404040404040;
constexpr bitboard file_h = 0x8080808080808080;
constexpr bitboard rank_1 = 0x00000000000000FF;
constexpr bitboard rank_2 = 0x000000000000FF00;
constexpr bitboard rank_3 = 0x0000000000FF0000;
constexpr bitboard rank_4 = 0x00000000FF000000;
constexpr bitboard rank_5 = 0x000000FF00000000;
constexpr bitboard rank_6 = 0x0000FF0000000000;
constexpr bitboard rank_7 = 0x00FF000000000000;
constexpr bitboard rank_8 = 0xFF00000000000000;

constexpr bitboard file_a_i = ~file_a;
constexpr bitboard file_b_i = ~file_b;
constexpr bitboard file_c_i = ~file_c;
constexpr bitboard file_d_i = ~file_d;
constexpr bitboard file_e_i = ~file_e;
constexpr bitboard file_f_i = ~file_f;
constexpr bitboard file_g_i = ~file_g;
constexpr bitboard file_h_i = ~file_h;
constexpr bitboard rank_1_i = ~rank_1;
constexpr bitboard rank_2_i = ~rank_2;
constexpr bitboard rank_3_i = ~rank_3;
constexpr bitboard rank_4_i = ~rank_4;
constexpr bitboard rank_5_i = ~rank_5;
constexpr bitboard rank_6_i = ~rank_6;
constexpr bitboard rank_7_i = ~rank_7;
constexpr bitboard rank_8_i = ~rank_8;

constexpr bitboard file_a_i_rank_1_i = file_a_i & rank_1_i;
constexpr bitboard file_a_i_rank_8_i = file_a_i & rank_8_i;
constexpr bitboard file_h_i_rank_1_i = file_h_i & rank_1_i;
constexpr bitboard file_h_i_rank_8_i = file_h_i & rank_8_i;
constexpr bitboard file_a_i_rank_8_i_rank_7_i = file_a_i_rank_8_i & rank_7_i;
constexpr bitboard file_a_i_rank_1_i_rank_2_i = file_a_i_rank_1_i & rank_2_i;
constexpr bitboard file_h_i_rank_8_i_rank_7_i = file_h_i_rank_8_i & rank_7_i;
constexpr bitboard file_h_i_rank_1_i_rank_2_i = file_h_i_rank_1_i & rank_2_i;
constexpr bitboard file_a_i_rank_8_i_file_b_i = file_a_i_rank_8_i & file_b_i;
constexpr bitboard file_a_i_rank_1_i_file_b_i = file_a_i_rank_1_i & file_b_i;
constexpr bitboard file_h_i_rank_8_i_file_g_i = file_h_i_rank_8_i & file_g_i;
constexpr bitboard file_h_i_rank_1_i_file_g_i = file_h_i_rank_1_i & file_g_i;

constexpr bitboard file[8] = {file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h};
constexpr bitboard rank[8] = {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};

constexpr bitboard dark_squares = file_a ^file_c ^file_e ^file_g ^rank_2 ^rank_4 ^rank_6 ^rank_8;
constexpr bitboard light_squares = ~dark_squares;

constexpr bitboard get_bb(unsigned int f, unsigned int r) {
    return 1uL << (8 * r + f);
}

constexpr bitboard get_bb(const square sq) {
    return sq == SQ_NONE ? 0 : (1uL << unsigned(sq));
}

inline square get_square(bitboard board) {
    return lsb(board);
}

inline bitboard king_attacks(square origin) {
    return piece_attacks_bb[KING][origin];
}

inline bitboard king_attacks(bitboard origin) {
    return king_attacks(get_square(origin));
}

inline bitboard knight_attacks(square origin) {
    return piece_attacks_bb[KNIGHT][origin];
}

inline bitboard knight_attacks(bitboard origin) {
    return knight_attacks(get_square(origin));
}

inline std::string bb_to_string(bitboard b) {
    std::stringstream ret;
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y + 1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard sq = get_bb(x, y);
            if ((b & sq) != 0) {
                ret << " X";
            } else {
                ret << " .";
            }
        }
        ret << std::endl;
    }
    ret << std::endl << "     a b c d e f g h" << std::endl;
    return ret.str();
}

inline void print_bb(bitboard b) {
    std::cout << "\n\n" << bb_to_string(b) << "\n\n";
}


constexpr bitboard operator&(bitboard b, square s) {
    return b & get_bb(s);
}

}


#endif //CHESSENGINE_BITBOARD_H
