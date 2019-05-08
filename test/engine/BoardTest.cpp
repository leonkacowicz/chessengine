#include <gtest/gtest.h>
#include <iostream>
#include <Board.h>
#include <SquarePosition.h>

TEST(BoardTest, Board_with_only_kings_is_not_checkmate) {
    Board board;
    ASSERT_FALSE(board.isCheckmate());
}

TEST(BoardTest, King_Attacks) {
    Board board;
    board.calculateAttacks();

    BitBoard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    ASSERT_EQ(expected_white_attacks, board.getAttacksFrom(WHITE));

    BitBoard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.getAttacksFrom(BLACK));
}

TEST(BoardTest, Rook_attacks) {
    Board board;
    SquarePosition c4("c4");
    board.putRook(WHITE, c4);
    board.calculateAttacks();
    BitBoard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    expected_white_attacks |= (fileC | rank4) & ~BitBoard(c4);
    ASSERT_EQ(expected_white_attacks, board.getAttacksFrom(WHITE));

    BitBoard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.getAttacksFrom(BLACK));
}

int main(int argc, char **argv) {
    BitBoard::initializePositions();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}