#include <gtest/gtest.h>
#include <square.h>
#include <bitboard.h>

TEST(SquarePositionTest, SquarePositionsCorrectness) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char file = static_cast<char>('a' + x);
            char rank = static_cast<char>('1' + y);
            const std::string square_str({file, rank});
            square square(x, y);
            ASSERT_EQ(x, square.get_file());
            ASSERT_EQ(y, square.get_rank());
            ASSERT_EQ(square_str, square.to_string());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromString) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            char file = static_cast<char>('a' + x);
            char rank = static_cast<char>('1' + y);
            const std::string square_str({file, rank});
            square square(square_str);
            ASSERT_EQ(x, square.get_file());
            ASSERT_EQ(y, square.get_rank());
            ASSERT_EQ(square_str, square.to_string());
        }
}

TEST(SquarePositionTest, SquarePositionsCorrectnessFromBitBoard) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            square expected(x, y);
            bitboard bitBoard = bitboard(expected);
            square calculated = bitBoard.asSquarePosition();
            ASSERT_EQ(expected, calculated);
        }
}