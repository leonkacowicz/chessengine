#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cstdint>
#include <ostream>
#include <initializer_list>
#include "Square.h"

using U64 = unsigned long int;

class bitboard {

    static Square squarePositions[67];
    U64 board;

public:

    bitboard() noexcept : board(0) {}

    bitboard(U64 mask) noexcept : board(mask) {};

    bitboard(unsigned int x, unsigned int y) noexcept : board(1uL << (8 * y + x)) {};

    bitboard(const Square square) noexcept : bitboard(square.get_file(), square.get_rank()) {}

    bitboard(const std::initializer_list<Square>& squares) noexcept {
        for (auto& pos : squares) {
            board |= 1uL << (8 * pos.get_rank() + pos.get_file());
        }
    }

    bitboard(const std::initializer_list<std::string>& positions) noexcept {
        board = 0;
        for (auto& pos : positions) {
            board |= bitboard(Square(pos)).board;
        }
    }

    bitboard shift_left(const unsigned int arg) const {
        return {board >> arg};
    }

    bitboard shift_right(const unsigned int arg) const {
        return {board << arg};
    }

    bitboard shift_up(const unsigned int arg) const {
        return {board << (8 * arg)};
    }

    bitboard shift_down(const unsigned int arg) const {
        return {board >> (8 * arg)};
    }

    bitboard shift_up_left(const unsigned int arg) const {
        return shift_up(arg).shift_left(arg);
    }

    bitboard shift_up_right(const unsigned int arg) const {
        return shift_up(arg).shift_right(arg);
    }

    bitboard shift_down_left(const unsigned int arg) const {
        return shift_down(arg).shift_left(arg);
    }

    bitboard shift_down_right(const unsigned int arg) const {
        return shift_down(arg).shift_right(arg);
    }

    bool operator==(const U64 rhs) const {
        return board == rhs;
    }

    bool operator==(const bitboard rhs) const {
        return board == rhs.board;
    }

    bitboard operator<<(const int rhs) const {
        return {board << rhs};
    }

    bitboard& operator<<=(const int rhs) {
        board <<= rhs;
        return *this;
    }

    bitboard operator>>(const int rhs) const {
        return {board >> rhs};
    }

    bitboard& operator>>=(const int rhs) {
        board >>= rhs;
        return *this;
    }

    bitboard operator|(const bitboard rhs) const {
        return {board | rhs.board};
    }

    bitboard operator|(const uint64_t rhs) const {
        return {board | rhs};
    }

    bitboard& operator|=(const bitboard rhs) {
        board |= rhs.board;
        return *this;
    }

    bitboard& operator|=(const U64 rhs) {
        board |= rhs;
        return *this;
    }

    bitboard& operator&=(const bitboard rhs) {
        board &= rhs.board;
        return *this;
    }

    bitboard& operator&=(const U64 rhs) {
        board &= rhs;
        return *this;
    }

    bitboard operator&(const bitboard rhs) const {
        return {board & rhs.board};
    }

    bitboard operator&(const uint64_t rhs) const {
        return {board & rhs};
    }

    bitboard operator~() const {
        return {~board};
    }

    bool operator[](const bitboard position) const {
        return (board & position.board) != 0;
    }

    bool operator[](const Square square) const {
        return (*this)[bitboard(square)];
    }

    bool isEmpty() const {
        return board == 0;
    }

    Square asSquarePosition() const {
        // https://www.chessprogramming.org/BitScan
        return squarePositions[(board & -board) % 67];
    }

    static void initializePositions() {
        // https://www.chessprogramming.org/BitScan
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            bitboard bitBoard(x, y);
            squarePositions[(bitBoard.board & -bitBoard.board) % 67] = Square(x, y);
        }
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

#endif //CHESSENGINE_BITBOARD_H