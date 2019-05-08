#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cstdint>
#include <ostream>
#include <initializer_list>
#include "SquarePosition.h"

using U64 = unsigned long int;

using namespace std;

class BitBoard {

    static SquarePosition squarePositions[67];
    U64 board;

public:

    BitBoard() noexcept : board(0) {}

    BitBoard(U64 mask) noexcept : board(mask) {};

    BitBoard(unsigned int x, unsigned int y) noexcept : board(1uL << (8 * y + x)) {};

    BitBoard(const SquarePosition position) noexcept : BitBoard(position.getX(), position.getY()) {}

    BitBoard(const initializer_list<SquarePosition>& positions) noexcept {
        for (auto& pos : positions) {
            board |= 1uL << (8 * pos.getY() + pos.getX());
        }
    }

    BitBoard(const initializer_list<string>& positions) noexcept {
        board = 0;
        for (auto& pos : positions) {
            board |= BitBoard(SquarePosition(pos)).board;
        }
    }

    BitBoard shiftLeft(const unsigned int arg) const {
        return {board >> arg};
    }

    BitBoard shiftRight(const unsigned int arg) const {
        return {board << arg};
    }

    BitBoard shiftUp(const unsigned int arg) const {
        return {board << (8 * arg)};
    }

    BitBoard shiftDown(const unsigned int arg) const {
        return {board >> (8 * arg)};
    }

    BitBoard shiftUpLeft(const unsigned int arg) const {
        return shiftUp(arg).shiftLeft(arg);
    }

    BitBoard shiftUpRight(const unsigned int arg) const {
        return shiftUp(arg).shiftRight(arg);
    }

    BitBoard shiftDownLeft(const unsigned int arg) const {
        return shiftDown(arg).shiftLeft(arg);
    }

    BitBoard shiftDownRight(const unsigned int arg) const {
        return shiftDown(arg).shiftRight(arg);
    }

    bool operator==(const U64 rhs) const {
        return board == rhs;
    }

    bool operator==(const BitBoard rhs) const {
        return board == rhs.board;
    }

    BitBoard operator<<(const int rhs) const {
        return {board << rhs};
    }

    BitBoard& operator<<=(const int rhs) {
        board <<= rhs;
        return *this;
    }

    BitBoard operator>>(const int rhs) const {
        return {board >> rhs};
    }

    BitBoard& operator>>=(const int rhs) {
        board >>= rhs;
        return *this;
    }

    BitBoard operator|(const BitBoard rhs) const {
        return {board | rhs.board};
    }

    BitBoard operator|(const uint64_t rhs) const {
        return {board | rhs};
    }

    BitBoard& operator|=(const BitBoard rhs) {
        board |= rhs.board;
        return *this;
    }

    BitBoard& operator|=(const U64 rhs) {
        board |= rhs;
        return *this;
    }

    BitBoard& operator&=(const BitBoard rhs) {
        board &= rhs.board;
        return *this;
    }

    BitBoard& operator&=(const U64 rhs) {
        board &= rhs;
        return *this;
    }

    BitBoard operator&(const BitBoard rhs) const {
        return {board & rhs.board};
    }

    BitBoard operator&(const uint64_t rhs) const {
        return {board & rhs};
    }

    BitBoard operator~() const {
        return {~board};
    }

    bool operator[](const BitBoard position) const {
        return (board & position.board) != 0;
    }

    bool operator[](const SquarePosition position) const {
        return (*this)[BitBoard(position)];
    }

    bool isEmpty() const {
        return board == 0;
    }

    SquarePosition asSquarePosition() const {
        // https://www.chessprogramming.org/BitScan
        return BitBoard::squarePositions[(board & -board) % 67];
    }

    static void initializePositions() {
        // https://www.chessprogramming.org/BitScan
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            BitBoard bitBoard(x, y);
            squarePositions[(bitBoard.board & -bitBoard.board) % 67] = SquarePosition(x, y);
        }
    }
};

static const BitBoard fileA = 0x0101010101010101;
static const BitBoard fileB = 0x0202020202020202;
static const BitBoard fileC = 0x0404040404040404;
static const BitBoard fileD = 0x0808080808080808;
static const BitBoard fileE = 0x1010101010101010;
static const BitBoard fileF = 0x2020202020202020;
static const BitBoard fileG = 0x4040404040404040;
static const BitBoard fileH = 0x8080808080808080;
static const BitBoard rank1 = 0x00000000000000FF;
static const BitBoard rank2 = 0x000000000000FF00;
static const BitBoard rank3 = 0x0000000000FF0000;
static const BitBoard rank4 = 0x00000000FF000000;
static const BitBoard rank5 = 0x000000FF00000000;
static const BitBoard rank6 = 0x0000FF0000000000;
static const BitBoard rank7 = 0x00FF000000000000;
static const BitBoard rank8 = 0xFF00000000000000;

#endif //CHESSENGINE_BITBOARD_H
