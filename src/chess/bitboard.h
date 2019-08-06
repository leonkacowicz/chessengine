#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cassert>
#include <cstdint>
#include <ostream>
#include <initializer_list>
#include <sstream>
#include "square.h"
#include "color.h"

typedef uint64_t bitboard;
using namespace chess::core;

enum shift_direction : int {
    UP = 8, DOWN = -8, LEFT = -1, RIGHT = 1,
    UP_LEFT = UP + LEFT, // not necessary but leaving them there as example: one could call shift<3 * UP + 2 * LEFT>(b)
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

constexpr bitboard king_attacks_[67] = {0x0000000000000000,0x0000000000000302,0x0000000000000705,0x0000c040c0000000,0x0000000000000e0a,
    0x0000000000c040c0,0x0003020300000000,0x00000000c040c000,0x0000000000001c14,0x0000000000382838,0x0000000003020300,0x141c000000000000,
    0x0007050700000000,0x000000001c141c00,0x0000000302030000,0xe0a0e00000000000,0x0000000000003828,0x0000000000000000,0x0000000000705070,
    0x00000000000e0a0e,0x0000000007050700,0xa0e0000000000000,0x2838000000000000,0x0000003828380000,0x000e0a0e00000000,0x000000e0a0e00000,
    0x0000000038283800,0x1c141c0000000000,0x0000000705070000,0x0038283800000000,0xc040c00000000000,0x00c040c000000000,0x0000000000007050,
    0x0000030203000000,0x0000000000000000,0x0000e0a0e0000000,0x0000000000e0a0e0,0x00000000e0a0e000,0x00000000001c141c,0x0a0e000000000000,
    0x000000000e0a0e00,0x7050700000000000,0x40c0000000000000,0x0000000000070507,0x5070000000000000,0x0000001c141c0000,0x0000007050700000,
    0x0e0a0e0000000000,0x001c141c00000000,0x00e0a0e000000000,0x000000c040c00000,0x0000705070000000,0x0000000070507000,0x0507000000000000,
    0x3828380000000000,0x0000000000030203,0x0000000e0a0e0000,0x0705070000000000,0x0070507000000000,0x0000382838000000,0x0203000000000000,
    0x000000000000c040,0x0302030000000000,0x00001c141c000000,0x000000000000e0a0,0x00000e0a0e000000,0x0000070507000000
};

constexpr bitboard knight_attacks_[67] = {0x0000000000000000,0x0000000000020400,0x0000000000050800,0x0040200020400000,0x00000000000a1100,
    0x0000000040200020,0x0204000402000000,0x0000004020002040,0x0000000000142200,0x0000000028440044,0x0000000204000402,0x0022140000000000,
    0x0508000805000000,0x0000001422002214,0x0000020400040200,0x100010a000000000,0x0000000000284400,0x0000000000000000,0x0000000050880088,
    0x000000000a110011,0x0000000508000805,0x0010a00000000000,0x0044280000000000,0x0000284400442800,0x0a1100110a000000,0x0000a0100010a000,
    0x0000002844004428,0x2200221400000000,0x0000050800080500,0x2844004428000000,0x2000204000000000,0x4020002040000000,0x0000000000508800,
    0x0002040004020000,0x0000000000000000,0x00a0100010a00000,0x00000000a0100010,0x000000a0100010a0,0x0000000014220022,0x00110a0000000000,
    0x0000000a1100110a,0x8800885000000000,0x0020400000000000,0x0000000005080008,0x0088500000000000,0x0000142200221400,0x0000508800885000,
    0x1100110a00000000,0x1422002214000000,0xa0100010a0000000,0x0000402000204000,0x0050880088500000,0x0000005088008850,0x0008050000000000,
    0x4400442800000000,0x0000000002040004,0x00000a1100110a00,0x0800080500000000,0x5088008850000000,0x0028440044280000,0x0004020000000000,
    0x0000000000402000,0x0400040200000000,0x0014220022140000,0x0000000000a01000,0x000a1100110a0000,0x0005080008050000
};

constexpr bitboard white_pawn_attacks_[67] = {0x0000000000000000,0x0000000000000200,0x0000000000000500,0x0000400000000000,0x0000000000000a00,
                                               0x0000000000400000,0x0002000000000000,0x0000000040000000,0x0000000000001400,0x0000000000280000,
                                               0x0000000002000000,0x0000000000000000,0x0005000000000000,0x0000000014000000,0x0000000200000000,
                                               0xa000000000000000,0x0000000000002800,0x0000000000000000,0x0000000000500000,0x00000000000a0000,
                                               0x0000000005000000,0x0000000000000000,0x0000000000000000,0x0000002800000000,0x000a000000000000,
                                               0x000000a000000000,0x0000000028000000,0x1400000000000000,0x0000000500000000,0x0028000000000000,
                                               0x4000000000000000,0x0040000000000000,0x0000000000005000,0x0000020000000000,0x0000000000000000,
                                               0x0000a00000000000,0x0000000000a00000,0x00000000a0000000,0x0000000000140000,0x0000000000000000,
                                               0x000000000a000000,0x5000000000000000,0x0000000000000000,0x0000000000050000,0x0000000000000000,
                                               0x0000001400000000,0x0000005000000000,0x0a00000000000000,0x0014000000000000,0x00a0000000000000,
                                               0x0000004000000000,0x0000500000000000,0x0000000050000000,0x0000000000000000,0x2800000000000000,
                                               0x0000000000020000,0x0000000a00000000,0x0500000000000000,0x0050000000000000,0x0000280000000000,
                                               0x0000000000000000,0x0000000000004000,0x0200000000000000,0x0000140000000000,0x000000000000a000,
                                               0x00000a0000000000,0x0000050000000000};

const bitboard black_pawn_attacks_[67] =  {0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000040000000,0x0000000000000000,
                                                0x0000000000000040,0x0000000200000000,0x0000000000004000,0x0000000000000000,0x0000000000000028,
                                                0x0000000000000200,0x0014000000000000,0x0000000500000000,0x0000000000001400,0x0000000000020000,
                                                0x0000a00000000000,0x0000000000000000,0x0000000000000000,0x0000000000000050,0x000000000000000a,
                                                0x0000000000000500,0x00a0000000000000,0x0028000000000000,0x0000000000280000,0x0000000a00000000,
                                                0x0000000000a00000,0x0000000000002800,0x0000140000000000,0x0000000000050000,0x0000002800000000,
                                                0x0000400000000000,0x0000004000000000,0x0000000000000000,0x0000000002000000,0x0000000000000000,
                                                0x00000000a0000000,0x00000000000000a0,0x000000000000a000,0x0000000000000014,0x000a000000000000,
                                                0x0000000000000a00,0x0000500000000000,0x0040000000000000,0x0000000000000005,0x0050000000000000,
                                                0x0000000000140000,0x0000000000500000,0x00000a0000000000,0x0000001400000000,0x000000a000000000,
                                                0x0000000000400000,0x0000000050000000,0x0000000000005000,0x0005000000000000,0x0000280000000000,
                                                0x0000000000000002,0x00000000000a0000,0x0000050000000000,0x0000005000000000,0x0000000028000000,
                                                0x0002000000000000,0x0000000000000000,0x0000020000000000,0x0000000014000000,0x0000000000000000,
                                                0x000000000a000000,0x0000000005000000};

constexpr bitboard pawn_attacks_[2][67] = {{0x0000000000000000,0x0000000000000200,0x0000000000000500,0x0000400000000000,0x0000000000000a00,
                                            0x0000000000400000,0x0002000000000000,0x0000000040000000,0x0000000000001400,0x0000000000280000,
                                            0x0000000002000000,0x0000000000000000,0x0005000000000000,0x0000000014000000,0x0000000200000000,
                                            0xa000000000000000,0x0000000000002800,0x0000000000000000,0x0000000000500000,0x00000000000a0000,
                                            0x0000000005000000,0x0000000000000000,0x0000000000000000,0x0000002800000000,0x000a000000000000,
                                            0x000000a000000000,0x0000000028000000,0x1400000000000000,0x0000000500000000,0x0028000000000000,
                                            0x4000000000000000,0x0040000000000000,0x0000000000005000,0x0000020000000000,0x0000000000000000,
                                            0x0000a00000000000,0x0000000000a00000,0x00000000a0000000,0x0000000000140000,0x0000000000000000,
                                            0x000000000a000000,0x5000000000000000,0x0000000000000000,0x0000000000050000,0x0000000000000000,
                                            0x0000001400000000,0x0000005000000000,0x0a00000000000000,0x0014000000000000,0x00a0000000000000,
                                            0x0000004000000000,0x0000500000000000,0x0000000050000000,0x0000000000000000,0x2800000000000000,
                                            0x0000000000020000,0x0000000a00000000,0x0500000000000000,0x0050000000000000,0x0000280000000000,
                                            0x0000000000000000,0x0000000000004000,0x0200000000000000,0x0000140000000000,0x000000000000a000,
                                            0x00000a0000000000,0x0000050000000000},
                                           {0x0000000000000000,0x0000000000000000,0x0000000000000000,0x0000000040000000,0x0000000000000000,
                                            0x0000000000000040,0x0000000200000000,0x0000000000004000,0x0000000000000000,0x0000000000000028,
                                            0x0000000000000200,0x0014000000000000,0x0000000500000000,0x0000000000001400,0x0000000000020000,
                                            0x0000a00000000000,0x0000000000000000,0x0000000000000000,0x0000000000000050,0x000000000000000a,
                                            0x0000000000000500,0x00a0000000000000,0x0028000000000000,0x0000000000280000,0x0000000a00000000,
                                            0x0000000000a00000,0x0000000000002800,0x0000140000000000,0x0000000000050000,0x0000002800000000,
                                            0x0000400000000000,0x0000004000000000,0x0000000000000000,0x0000000002000000,0x0000000000000000,
                                            0x00000000a0000000,0x00000000000000a0,0x000000000000a000,0x0000000000000014,0x000a000000000000,
                                            0x0000000000000a00,0x0000500000000000,0x0040000000000000,0x0000000000000005,0x0050000000000000,
                                            0x0000000000140000,0x0000000000500000,0x00000a0000000000,0x0000001400000000,0x000000a000000000,
                                            0x0000000000400000,0x0000000050000000,0x0000000000005000,0x0005000000000000,0x0000280000000000,
                                            0x0000000000000002,0x00000000000a0000,0x0000050000000000,0x0000005000000000,0x0000000028000000,
                                            0x0002000000000000,0x0000000000000000,0x0000020000000000,0x0000000014000000,0x0000000000000000,
                                            0x000000000a000000,0x0000000005000000}
                                            };

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

constexpr bitboard bb(unsigned int f, unsigned int r) {
    return 1uL << (8 * r + f);
}

constexpr bitboard bb(const square sq) {
    return sq == SQ_NONE ? 0 : bb(get_file(sq), get_rank(sq));
    // since everything is constexpr this will be 1uL << sq, but leaving like this to prevent future corretude questioning in the future
}

inline square get_square(bitboard board) {
    return lsb(board);
}

inline bitboard king_attacks(bitboard origin) {
    return king_attacks_[(origin & -origin) % 67];
}

inline bitboard knight_attacks(bitboard origin) {
    return knight_attacks_[(origin & -origin) % 67];
}

inline bitboard pawn_attacks(bitboard origin, color c) {
    return pawn_attacks_[c][(origin & -origin) % 67];
}

inline std::string bb_to_string(bitboard b) {
    std::stringstream ret;
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y + 1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard sq = bb(x, y);
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
    return b & bb(s);
}

#endif //CHESSENGINE_BITBOARD_H
