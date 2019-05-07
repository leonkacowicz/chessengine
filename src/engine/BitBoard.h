#ifndef CHESSENGINE_BITBOARD_H
#define CHESSENGINE_BITBOARD_H

#include <cstdint>
#include <ostream>
#include "SquarePosition.h"

using U64 = unsigned long int;

using namespace std;

class BitBoard {
    U64 board;

public:
    static SquarePosition squarePositions[67];

    BitBoard() noexcept : board(0) {}

    BitBoard(U64 mask) noexcept : board(mask) {};

    BitBoard(unsigned int x, unsigned int y) noexcept : board(1uL << (7 - x) << (y << 3u)) {};

    BitBoard(const SquarePosition position) noexcept : BitBoard(position.getX(), position.getY()) {}

    static BitBoard fromByteArray(const unsigned char ranks8to1[8]) {
        return {(U64)(((((((
                (U64)ranks8to1[0] << 8u |
                (U64)ranks8to1[1]) << 8u |
                (U64)ranks8to1[2]) << 8u |
                (U64)ranks8to1[3]) << 8u |
                (U64)ranks8to1[4]) << 8u |
                (U64)ranks8to1[5]) << 8u |
                (U64)ranks8to1[6]) << 8u |
                (U64)ranks8to1[7])};
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

    bool operator[](const SquarePosition position) const {
        return (board & (1L << (7 - position.getX()) << (position.getY() << 3))) != 0;
    }

    bool operator[](const BitBoard position) const {
        return (board & position.board) != 0;
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


#endif //CHESSENGINE_BITBOARD_H
