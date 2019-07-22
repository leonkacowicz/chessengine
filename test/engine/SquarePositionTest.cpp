#include <gtest/gtest.h>
#include <Square.h>
#include <bitboard.h>

TEST(SquarePositionTest, SquarePositionsCorrectness) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char file = static_cast<char>('a' + x);
            char rank = static_cast<char>('1' + y);
            const std::string square_str({file, rank});
            Square square(x, y);
            ASSERT_EQ(x, square.getX());
            ASSERT_EQ(y, square.getY());
            ASSERT_EQ(square_str, square.to_string());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromString) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char file = static_cast<char>('a' + x);
            char rank = static_cast<char>('1' + y);
            const std::string square_str({file, rank});
            Square square(square_str);
            ASSERT_EQ(x, square.getX());
            ASSERT_EQ(y, square.getY());
            ASSERT_EQ(square_str, square.to_string());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromBitBoard) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            Square expected(x, y);
            bitboard bitBoard = bitboard(expected);
            Square calculated = bitBoard.asSquarePosition();
            ASSERT_EQ(expected, calculated);
        }
}