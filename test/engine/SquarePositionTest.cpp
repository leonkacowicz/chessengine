#include <gtest/gtest.h>
#include <Square.h>
#include <BitBoard.h>

TEST(SquarePositionTest, SquarePositionsCorrectness) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            Square square(x, y);
            ASSERT_EQ(x, square.getX());
            ASSERT_EQ(y, square.getY());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromString) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char file = static_cast<char>('a' + x);
            char rank = static_cast<char>('1' + y);
            Square square(std::string({file, rank}));
            ASSERT_EQ(x, square.getX());
            ASSERT_EQ(y, square.getY());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromBitBoard) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            Square expected(x, y);
            BitBoard bitBoard = BitBoard(expected);
            Square calculated = bitBoard.asSquarePosition();
            ASSERT_EQ(expected, calculated);
        }
}