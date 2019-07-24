#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <board.h>
#include <square.h>
#include "../test_common.h"

TEST(board_test, Board_with_only_kings_is_not_checkmate) {
    board board;
    ASSERT_FALSE(board.is_checkmate());
}

TEST(board_test, King_Attacks) {
    board board;
    board.calculate_attacks();

    bitboard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    ASSERT_EQ(expected_white_attacks, board.get_attacks(WHITE));

    bitboard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.get_attacks(BLACK));
}

TEST(board_test, Rook_attacks) {
    board board;
    board.put_piece(ROOK, WHITE, "c4");
    board.calculate_attacks();
    bitboard expected_white_attacks{"d1", "d2", "e2", "f2", "f1"};
    expected_white_attacks |= (file_c | rank_4) & ~bitboard("c4");
    ASSERT_EQ(expected_white_attacks, board.get_attacks(WHITE));

    bitboard expected_black_attacks{"d8", "d7", "e7", "f7", "f8"};
    ASSERT_EQ(expected_black_attacks, board.get_attacks(BLACK));
}


TEST(board_test, legal_moves_king_cannot_move_into_check) {
    board board;

    board.set_king_position(WHITE, {"g5"});
    board.set_king_position(BLACK, {"a1"});
    board.put_piece(ROOK, BLACK, {"f1"});
    const std::vector<move> moves = board.get_legal_moves(WHITE);

//    ASSERT_EQ(std::find(moves.begin(), moves.end(), move({"g5"}, {"f6"})), moves.end());
//    ASSERT_EQ(std::find(moves.begin(), moves.end(), move({"g5"}, {"f5"})), moves.end());
//    ASSERT_EQ(std::find(moves.begin(), moves.end(), move({"g5"}, {"f4"})), moves.end());
//
//    ASSERT_NE(std::find(moves.begin(), moves.end(), move({"g5"}, {"g6"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), move({"g5"}, {"h6"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), move({"g5"}, {"h5"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), move({"g5"}, {"h4"})), moves.end());
//    ASSERT_NE(std::find(moves.begin(), moves.end(), move({"g5"}, {"g4"})), moves.end());
}

TEST(board_test, rook_pinned_by_rook_from_above) {
    board b;
    b.set_king_position(WHITE, "e1");
    b.set_king_position(BLACK, "e8");
    b.put_piece(ROOK, WHITE, "e2");
    b.put_piece(ROOK, BLACK, "e7");

    b.calculate_attacks();

    const std::vector<move>& moves = b.get_legal_moves(WHITE);

    for (auto& move: moves) {
        std::cout << move.origin.to_string() << move.destination.to_string() << std::endl;
    }

    ASSERT_NOT_CONTAINS(moves, move("e2", "a2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "b2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "c2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "d2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "f2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "g2"));
    ASSERT_NOT_CONTAINS(moves, move("e2", "h2"));
}

TEST(board_test, print_sizeof_board) {
    std::cout << sizeof(board) << std::endl;
}