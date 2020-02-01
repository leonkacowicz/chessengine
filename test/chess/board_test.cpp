#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <chess/board.h>
#include <chess/square.h>
#include <chess/fen.h>
#include <chess/move_gen.h>
#include "../test_common.h"

using namespace chess::core;

void list_moves(const std::vector<move>& moves) {
    std::cout << "Moves found:" << std::endl;
    for (auto& m: moves) {
        std::cout << to_long_move(m) << std::endl;
    }
}

TEST(board_test, size_of_class) {
    std::cout << "\nSize of class: " << sizeof(board) << " bytes" << std::endl;
}

TEST(board_test, fen_test) {
    board b;
    b.set_initial_position();
    ASSERT_EQ(fen::to_string(b), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_EQ(b,  fen::board_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    b.make_move(get_move("e2", "e4"));
    ASSERT_EQ(fen::to_string(b), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    ASSERT_EQ(b, fen::board_from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"));
    b.make_move(get_move("c7", "c5"));
    ASSERT_EQ(fen::to_string(b, 2), "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
    ASSERT_EQ(b, fen::board_from_fen("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"));
    b.make_move(get_move("g1", "f3"));
    ASSERT_EQ(fen::to_string(b, 2), "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    ASSERT_EQ(b, fen::board_from_fen("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"));
}


TEST(not_under_check, knight_on_a3) {
    board b;
    b.set_initial_position();
    b.print();
    //b.make_get_move({"b1", "a3"});
    ASSERT_FALSE(b.under_check(WHITE));
}


TEST(board_test, under_check_specific_pos1) {
    /*
 8   . . . . . . . .
 7   . . . . k p . p
 6   . . P . b . . .
 5   . n r . . p . .
 4   . . . . . . . .
 3   . . . . . B P .
 2   . . . R N K . P
 1   . . . . . . . .

     a b c d e f g h
     */

    board b;
    b.put_piece(ROOK, WHITE, SQ_D2);
    b.put_piece(KNIGHT, WHITE, SQ_E2);
    b.set_king_position(WHITE, SQ_F2);
    b.put_piece(PAWN, WHITE, SQ_H2);
    b.put_piece(PAWN, WHITE, SQ_G3);
    b.put_piece(BISHOP, WHITE, SQ_F3);
    b.put_piece(KNIGHT, BLACK, SQ_B5);
    b.put_piece(ROOK, BLACK, SQ_C5);
    b.put_piece(PAWN, BLACK, SQ_F5);
    b.put_piece(PAWN, WHITE, SQ_C6);
    b.put_piece(BISHOP, BLACK, SQ_E6);
    b.set_king_position(BLACK, SQ_E7);
    b.put_piece(PAWN, BLACK, SQ_F7);
    b.put_piece(PAWN, BLACK, SQ_H7);

    b.print();
    ASSERT_FALSE(b.under_check(WHITE));
}

TEST(board_test, assert_its_checkmate) {
    /*
 8   . . . . . . . k
 7   . . . n . . . .
 6   p . p . . p . Q
 5   P . . . . . . .
 4   . . B P . . . .
 3   . . P . . . . P
 2   . . . . p . P .
 1   . . . . . . K .

     a b c d e f g h
     */

    board b = chess::core::fen::board_from_fen("7k/3n4/p1p2p1Q/P7/2BP4/2P4P/4p1P1/6K1 b - - 0 1");
//    b.set_king_position(WHITE, SQ_G1);
//    b.put_piece(PAWN, BLACK, SQ_E2);
//    b.put_piece(PAWN, WHITE, SQ_G2);
//    b.put_piece(PAWN, WHITE, SQ_C3);
//    b.put_piece(PAWN, WHITE, SQ_H3);
//    b.put_piece(BISHOP, WHITE, SQ_C4);
//    b.put_piece(PAWN, WHITE, SQ_D4);
//    b.put_piece(PAWN, WHITE, SQ_A5);
//    b.put_piece(PAWN, BLACK, SQ_A6);
//    b.put_piece(PAWN, BLACK, SQ_C6);
//    b.put_piece(PAWN, BLACK, SQ_F6);
//    b.put_piece(QUEEN, WHITE, SQ_H6);
//    b.put_piece(KNIGHT, BLACK, SQ_D7);
//    b.set_king_position(BLACK, SQ_H8);

    ASSERT_TRUE(b.under_check(BLACK));
    const std::vector<move>& moves = move_gen(b).generate();
    list_moves(moves);
    ASSERT_TRUE(moves.empty());
}

TEST(board_test, print_sizeof_board) {
    std::cout << sizeof(board) << std::endl;
}

TEST(board_test, flip_colors_test) {

    board b;
    b.set_king_position(color::WHITE, SQ_E1);
    b.put_piece(piece::ROOK, color::WHITE, SQ_H1);
    b.put_piece(piece::PAWN, color::WHITE, SQ_A2);
    b.put_piece(piece::PAWN, color::WHITE, SQ_B2);
    b.put_piece(piece::KNIGHT, color::WHITE, SQ_H5);
    b.set_king_position(color::BLACK, SQ_A8);
    b.put_piece(piece::PAWN, color::BLACK, SQ_A7);
    b.put_piece(piece::QUEEN, color::BLACK, SQ_B8);
    b.print();
    b.can_castle_king_side[color::WHITE] = true;
    std::cout << fen::to_string(b) << std::endl;
    board f = b.flip_colors();
    f.print();
    std::cout << fen::to_string(f) << std::endl;

}