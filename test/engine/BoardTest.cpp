#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <Board.h>
#include <Square.h>

TEST(BoardTest, Board_with_only_kings_is_not_checkmate) {
    Board board;
    ASSERT_FALSE(board.isCheckmate());
}

TEST(BoardTest, King_Attacks) {
    Board board;
    board.calculateAttacks();

    bitboard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    ASSERT_EQ(expected_white_attacks, board.getAttacksFrom(WHITE));

    bitboard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.getAttacksFrom(BLACK));
}

TEST(BoardTest, Rook_attacks) {
    Board board;
    Square c4("c4");
    board.putRook(WHITE, {"c4"});
    board.calculateAttacks();
    bitboard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    expected_white_attacks |= (file_c | rank_4) & ~bitboard(c4);
    ASSERT_EQ(expected_white_attacks, board.getAttacksFrom(WHITE));

    bitboard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.getAttacksFrom(BLACK));
}


TEST(BoardTest, legal_moves_king_cannot_move_into_check) {
    Board board;

    board.putKing(WHITE, {"g5"});
    board.putKing(BLACK, {"a1"});
    board.putRook(BLACK, {"f1"});
    const std::vector<Move> moves = board.getPossibleMovesFor(WHITE);

//    ASSERT_EQ(std::find(moves.begin(), moves.end(), Move({"g5"}, {"f6"})), moves.end());
//    ASSERT_EQ(std::find(moves.begin(), moves.end(), Move({"g5"}, {"f5"})), moves.end());
//    ASSERT_EQ(std::find(moves.begin(), moves.end(), Move({"g5"}, {"f4"})), moves.end());
//
//    ASSERT_NE(std::find(moves.begin(), moves.end(), Move({"g5"}, {"g6"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), Move({"g5"}, {"h6"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), Move({"g5"}, {"h5"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), Move({"g5"}, {"h4"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), Move({"g5"}, {"g4"})), moves.end());
}

int main(int argc, char **argv) {
    bitboard::initializePositions();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}