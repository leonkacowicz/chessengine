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

    unsigned char expectedAttacksFromWhite[] = {
            0b00000000,
            0b00000000,
            0b00000000,
            0b00000000,
            0b00000000,
            0b00000000,
            0b00011100,
            0b00010100
    };
    ASSERT_EQ(BitBoard::fromByteArray(expectedAttacksFromWhite), board.getAttacksFrom(WHITE));

    unsigned char expectedAttacksFromBlack[] = {
                                    0b0001'0100,
                                    0b0001'1100,
                                    0b0000'0000,
                                    0b0000'0000,
                                    0b0000'0000,
                                    0b0000'0000,
                                    0b0000'0000,
                                    0b0000'0000
                            };
    ASSERT_EQ(BitBoard::fromByteArray(expectedAttacksFromBlack), board.getAttacksFrom(BLACK));
}

TEST(BoardTest, Rook_attacks) {
    Board board;

    board.putRook(WHITE, SquarePosition("c4"));
    board.calculateAttacks();
    BitBoard attacks = board.getAttacksFrom(WHITE);

    unsigned char expectedAttacksFromWhite[] = {
            0b00100000,
            0b00100000,
            0b00100000,
            0b00100000,
            0b11011111,
            0b00100000,
            0b00111100,
            0b00110100
    };

    ASSERT_EQ(BitBoard::fromByteArray(expectedAttacksFromWhite), attacks);

    unsigned char expectedAttacksFromBlack[] = {
            0b0001'0100,
            0b0001'1100,
            0b0000'0000,
            0b0000'0000,
            0b0000'0000,
            0b0000'0000,
            0b0000'0000,
            0b0000'0000
    };
    ASSERT_EQ(BitBoard::fromByteArray(expectedAttacksFromBlack), board.getAttacksFrom(BLACK));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}