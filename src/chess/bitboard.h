#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cstdint>
#include <ostream>
#include <initializer_list>
#include <sstream>
#include "square.h"
#include "color.h"

using U64 = unsigned long int;

enum shift_direction {
    UP, DOWN, LEFT, RIGHT,
    UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT,
    UP_UP_LEFT, UP_LEFT_LEFT, UP_UP_RIGHT, UP_RIGHT_RIGHT, DOWN_DOWN_LEFT, DOWN_LEFT_LEFT, DOWN_DOWN_RIGHT, DOWN_RIGHT_RIGHT,
    LEFT_LEFT, RIGHT_RIGHT
};

namespace ns {
    template<shift_direction d> U64 shift(U64 arg);
    template <>
    inline U64 shift<UP>(U64 arg) {
        return arg << 8u;
    }
    template <>
    inline U64 shift<DOWN>(U64 arg) {
        return arg >> 8u;
    }
    template <>
    inline U64 shift<LEFT>(U64 arg) {
        return arg >> 1u;
    }
    template <>
    inline U64 shift<RIGHT>(U64 arg) {
        return arg << 1u;
    }

    template <>
    inline U64 shift<LEFT_LEFT>(U64 arg) {
        return arg >> 2u;
    }
    template <>
    inline U64 shift<RIGHT_RIGHT>(U64 arg) {
        return arg << 2u;
    }
    template <>
    inline U64 shift<UP_LEFT>(U64 arg) {
        return arg << 7u;
    }
    template <>
    inline U64 shift<UP_RIGHT>(U64 arg) {
        return arg << 9u;
    }
    template <>
    inline U64 shift<DOWN_LEFT>(U64 arg) {
        return arg >> 9u;
    }
    template <>
    inline U64 shift<DOWN_RIGHT>(U64 arg) {
        return arg >> 7u;
    }

    template <>
    inline U64 shift<UP_UP_LEFT>(U64 arg) {
        return arg << 15u;
    }
    template <>
    inline U64 shift<UP_LEFT_LEFT>(U64 arg) {
        return arg << 6u;
    }
    template <>
    inline U64 shift<UP_UP_RIGHT>(U64 arg) {
        return arg << 17u;
    }
    template <>
    inline U64 shift<UP_RIGHT_RIGHT>(U64 arg) {
        return arg << 10u;
    }
    template <>
    inline U64 shift<DOWN_DOWN_LEFT>(U64 arg) {
        return arg >> 17u;
    }
    template <>
    inline U64 shift<DOWN_LEFT_LEFT>(U64 arg) {
        return arg >> 10u;
    }
    template <>
    inline U64 shift<DOWN_DOWN_RIGHT>(U64 arg) {
        return arg >> 15u;
    }
    template <>
    inline U64 shift<DOWN_RIGHT_RIGHT>(U64 arg) {
        return arg >> 6u;
    }
}

class bitboard {

    static square squarePositions[67];
    static bitboard king_attacks_[67];
    static bitboard knight_attacks_[67];
    static bitboard pawn_attacks_[2][67];
    static bitboard white_pawn_attacks_[67];
    static bitboard black_pawn_attacks_[67];
public:
    U64 board = 0;

    inline bitboard() noexcept : board(0) {}

    inline bitboard(U64 mask) noexcept : board(mask) {};

    inline bitboard(unsigned int x, unsigned int y) noexcept : board(1uL << (8 * y + x)) {};

    inline bitboard(const square sq) noexcept : bitboard(sq == square::none ? 0 : bitboard(sq.get_file(), sq.get_rank())) {}

    inline bitboard shift_left(const unsigned int arg) const {
        return {board >> arg};
    }

    inline bitboard shift_right(const unsigned int arg) const {
        return {board << arg};
    }

    inline bitboard shift_up(const unsigned int arg) const {
        return {board << (8 * arg)};
    }

    inline bitboard shift_down(const unsigned int arg) const {
        return {board >> (8 * arg)};
    }

    template <shift_direction d>
    bitboard shift() const {
        return ns::shift<d>(board);
    }

    inline bool operator==(const U64 rhs) const {
        return board == rhs;
    }

    inline bool operator==(const bitboard rhs) const {
        return board == rhs.board;
    }

    inline bool operator!=(const U64 rhs) const {
        return board != rhs;
    }

    inline bool operator!=(const bitboard rhs) const {
        return board != rhs.board;
    }

    inline bitboard operator<<(const int rhs) const {
        return {board << rhs};
    }

    inline bitboard& operator<<=(const int rhs) {
        board <<= rhs;
        return *this;
    }

    inline bitboard operator>>(const int rhs) const {
        return {board >> rhs};
    }

    inline bitboard& operator>>=(const int rhs) {
        board >>= rhs;
        return *this;
    }

    inline bitboard operator|(const bitboard rhs) const {
        return {board | rhs.board};
    }

    inline bitboard operator|(const uint64_t rhs) const {
        return {board | rhs};
    }

    inline bitboard& operator|=(const bitboard rhs) {
        board |= rhs.board;
        return *this;
    }

    inline bitboard& operator|=(const U64 rhs) {
        board |= rhs;
        return *this;
    }

    inline bitboard& operator&=(const bitboard rhs) {
        board &= rhs.board;
        return *this;
    }

    inline bitboard& operator&=(const U64 rhs) {
        board &= rhs;
        return *this;
    }

    inline bitboard operator&(const bitboard rhs) const {
        return {board & rhs.board};
    }

    inline bitboard operator&(const uint64_t rhs) const {
        return {board & rhs};
    }

    inline bitboard operator~() const {
        return {~board};
    }

    inline bool operator[](const bitboard position) const {
        return (board & position.board) != 0;
    }

    inline bool empty() const {
        return board == 0;
    }

    inline square get_square() const {
        // https://www.chessprogramming.org/BitScan
        return squarePositions[(board & -board) % 67];
    }

    static inline bitboard king_attacks(bitboard origin) {
        return king_attacks_[(origin.board & -origin.board) % 67];
    }

    static inline bitboard knight_attacks(bitboard origin) {
        return knight_attacks_[(origin.board & -origin.board) % 67];
    }

    static inline bitboard white_pawn_attacks(bitboard origin) {
        return white_pawn_attacks_[(origin.board & -origin.board) % 67];
    }

    static inline bitboard black_pawn_attacks(bitboard origin) {
        return black_pawn_attacks_[(origin.board & -origin.board) % 67];
    }

    static inline bitboard pawn_attacks(bitboard origin, color c) {
        return pawn_attacks_[c][(origin.board & -origin.board) % 67];
    }

    std::string to_string() const {
        std::stringstream ret;
        for (int y = 7; y >= 0; y--) {
            ret << " " << (y + 1) << "  ";
            for (int x = 0; x <= 7; x++) {
                bitboard position(x, y);
                if ((board & position.board) != 0) {
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

    void print() const {
        std::cout << "\n\n" << to_string() << "\n\n";
    }
};

static const bitboard file_a(0x0101010101010101);
static const bitboard file_b(0x0202020202020202);
static const bitboard file_c(0x0404040404040404);
static const bitboard file_d(0x0808080808080808);
static const bitboard file_e(0x1010101010101010);
static const bitboard file_f(0x2020202020202020);
static const bitboard file_g(0x4040404040404040);
static const bitboard file_h(0x8080808080808080);
static const bitboard rank_1(0x00000000000000FF);
static const bitboard rank_2(0x000000000000FF00);
static const bitboard rank_3(0x0000000000FF0000);
static const bitboard rank_4(0x00000000FF000000);
static const bitboard rank_5(0x000000FF00000000);
static const bitboard rank_6(0x0000FF0000000000);
static const bitboard rank_7(0x00FF000000000000);
static const bitboard rank_8(0xFF00000000000000);

static const bitboard file_a_i(~file_a);
static const bitboard file_b_i(~file_b);
static const bitboard file_c_i(~file_c);
static const bitboard file_d_i(~file_d);
static const bitboard file_e_i(~file_e);
static const bitboard file_f_i(~file_f);
static const bitboard file_g_i(~file_g);
static const bitboard file_h_i(~file_h);
static const bitboard rank_1_i(~rank_1);
static const bitboard rank_2_i(~rank_2);
static const bitboard rank_3_i(~rank_3);
static const bitboard rank_4_i(~rank_4);
static const bitboard rank_5_i(~rank_5);
static const bitboard rank_6_i(~rank_6);
static const bitboard rank_7_i(~rank_7);
static const bitboard rank_8_i(~rank_8);

static const bitboard file_a_i_rank_1_i = file_a_i & rank_1_i;
static const bitboard file_a_i_rank_8_i = file_a_i & rank_8_i;
static const bitboard file_h_i_rank_1_i = file_h_i & rank_1_i;
static const bitboard file_h_i_rank_8_i = file_h_i & rank_8_i;

static const bitboard file_a_i_rank_8_i_rank_7_i = file_a_i_rank_8_i & rank_7_i;
static const bitboard file_a_i_rank_1_i_rank_2_i = file_a_i_rank_1_i & rank_2_i;
static const bitboard file_h_i_rank_8_i_rank_7_i = file_h_i_rank_8_i & rank_7_i;
static const bitboard file_h_i_rank_1_i_rank_2_i = file_h_i_rank_1_i & rank_2_i;
static const bitboard file_a_i_rank_8_i_file_b_i = file_a_i_rank_8_i & file_b_i;
static const bitboard file_a_i_rank_1_i_file_b_i = file_a_i_rank_1_i & file_b_i;
static const bitboard file_h_i_rank_8_i_file_g_i = file_h_i_rank_8_i & file_g_i;
static const bitboard file_h_i_rank_1_i_file_g_i = file_h_i_rank_1_i & file_g_i;

static const bitboard file[8] = {file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h};
static const bitboard rank[8] = {rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8};

#endif //CHESSENGINE_BITBOARD_H
