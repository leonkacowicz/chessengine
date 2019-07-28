#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <board.h>
#include <square.h>
#include "../test_common.h"

void list_moves(const std::vector<move>& moves) {
    std::cout << "Moves found:" << std::endl;
    for (auto& move: moves) {
        std::cout << move.to_long_move() << std::endl;
//        std::cout << move.origin.to_string() << move.destination.to_string() << std::endl;
    }
}

TEST(board_test, size_of_class) {
    std::cout << "\nSize of class: " << sizeof(board) << " bytes" << std::endl;
}

TEST(legal_moves_king, king_first_rank) {
    for (color c : {WHITE, BLACK}) {
        board b;
        b.set_king_position(c, "e1");
        const std::vector<move> &moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move("e1", "d1"));
        ASSERT_CONTAINS(moves, move("e1", "d2"));
        ASSERT_CONTAINS(moves, move("e1", "e2"));
        ASSERT_CONTAINS(moves, move("e1", "f2"));
        ASSERT_CONTAINS(moves, move("e1", "f1"));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_king, king_last_rank) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("e8");
        b.set_king_position(c, pos);
        const std::vector<move> &moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "d8"));
        ASSERT_CONTAINS(moves, move(pos, "d7"));
        ASSERT_CONTAINS(moves, move(pos, "e7"));
        ASSERT_CONTAINS(moves, move(pos, "f7"));
        ASSERT_CONTAINS(moves, move(pos, "f8"));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_king, king_h1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("h1");
        b.set_king_position(c, pos);
        const std::vector<move> &moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "h2"));
        ASSERT_CONTAINS(moves, move(pos, "g2"));
        ASSERT_CONTAINS(moves, move(pos, "g1"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_a1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("a1");
        b.set_king_position(c, pos);
        auto moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a2"));
        ASSERT_CONTAINS(moves, move(pos, "b2"));
        ASSERT_CONTAINS(moves, move(pos, "b1"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_a8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("a8");
        b.set_king_position(c, pos);
        auto moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a7"));
        ASSERT_CONTAINS(moves, move(pos, "b7"));
        ASSERT_CONTAINS(moves, move(pos, "b8"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_h8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("h8");
        b.set_king_position(c, pos);
        auto moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "h7"));
        ASSERT_CONTAINS(moves, move(pos, "g7"));
        ASSERT_CONTAINS(moves, move(pos, "g8"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_e5) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("e5");
        b.set_king_position(c, pos);
        auto moves = b.get_legal_moves(c);

        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "d4"));
        ASSERT_CONTAINS(moves, move(pos, "d5"));
        ASSERT_CONTAINS(moves, move(pos, "d6"));
        ASSERT_CONTAINS(moves, move(pos, "e4"));
        ASSERT_CONTAINS(moves, move(pos, "e6"));
        ASSERT_CONTAINS(moves, move(pos, "f4"));
        ASSERT_CONTAINS(moves, move(pos, "f5"));
        ASSERT_CONTAINS(moves, move(pos, "f6"));
        ASSERT_EQ(moves.size(), 8);
    }
}

TEST(legal_moves_rook, rook_a1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("a1");
        b.put_piece(ROOK, c, pos);
        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a2"));
        ASSERT_CONTAINS(moves, move(pos, "a3"));
        ASSERT_CONTAINS(moves, move(pos, "a4"));
        ASSERT_CONTAINS(moves, move(pos, "a5"));
        ASSERT_CONTAINS(moves, move(pos, "a6"));
        ASSERT_CONTAINS(moves, move(pos, "a7"));
        ASSERT_CONTAINS(moves, move(pos, "a8"));
        ASSERT_CONTAINS(moves, move(pos, "b1"));
        ASSERT_CONTAINS(moves, move(pos, "c1"));
        ASSERT_CONTAINS(moves, move(pos, "d1"));
        ASSERT_CONTAINS(moves, move(pos, "e1"));
        ASSERT_CONTAINS(moves, move(pos, "f1"));
        ASSERT_CONTAINS(moves, move(pos, "g1"));
        ASSERT_CONTAINS(moves, move(pos, "h1"));
        ASSERT_EQ(moves.size(), 14);
    }
}

TEST(legal_moves_rook, rook_a8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("a8");
        b.put_piece(ROOK, c, pos);
        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a2"));
        ASSERT_CONTAINS(moves, move(pos, "a3"));
        ASSERT_CONTAINS(moves, move(pos, "a4"));
        ASSERT_CONTAINS(moves, move(pos, "a5"));
        ASSERT_CONTAINS(moves, move(pos, "a6"));
        ASSERT_CONTAINS(moves, move(pos, "a7"));
        ASSERT_CONTAINS(moves, move(pos, "a1"));
        ASSERT_CONTAINS(moves, move(pos, "b8"));
        ASSERT_CONTAINS(moves, move(pos, "c8"));
        ASSERT_CONTAINS(moves, move(pos, "d8"));
        ASSERT_CONTAINS(moves, move(pos, "e8"));
        ASSERT_CONTAINS(moves, move(pos, "f8"));
        ASSERT_CONTAINS(moves, move(pos, "g8"));
        ASSERT_CONTAINS(moves, move(pos, "h8"));
        ASSERT_EQ(moves.size(), 14);
    }
}

TEST(legal_moves_rook, rook_h8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("h8");
        b.put_piece(ROOK, c, pos);
        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "h2"));
        ASSERT_CONTAINS(moves, move(pos, "h3"));
        ASSERT_CONTAINS(moves, move(pos, "h4"));
        ASSERT_CONTAINS(moves, move(pos, "h5"));
        ASSERT_CONTAINS(moves, move(pos, "h6"));
        ASSERT_CONTAINS(moves, move(pos, "h7"));
        ASSERT_CONTAINS(moves, move(pos, "a8"));
        ASSERT_CONTAINS(moves, move(pos, "b8"));
        ASSERT_CONTAINS(moves, move(pos, "c8"));
        ASSERT_CONTAINS(moves, move(pos, "d8"));
        ASSERT_CONTAINS(moves, move(pos, "e8"));
        ASSERT_CONTAINS(moves, move(pos, "f8"));
        ASSERT_CONTAINS(moves, move(pos, "g8"));
        ASSERT_CONTAINS(moves, move(pos, "h1"));
        ASSERT_EQ(moves.size(), 14);
    }
}

TEST(legal_moves_rook, rook_h1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("h1");
        b.put_piece(ROOK, c, pos);
        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "h2"));
        ASSERT_CONTAINS(moves, move(pos, "h3"));
        ASSERT_CONTAINS(moves, move(pos, "h4"));
        ASSERT_CONTAINS(moves, move(pos, "h5"));
        ASSERT_CONTAINS(moves, move(pos, "h6"));
        ASSERT_CONTAINS(moves, move(pos, "h7"));
        ASSERT_CONTAINS(moves, move(pos, "h8"));
        ASSERT_CONTAINS(moves, move(pos, "a1"));
        ASSERT_CONTAINS(moves, move(pos, "b1"));
        ASSERT_CONTAINS(moves, move(pos, "c1"));
        ASSERT_CONTAINS(moves, move(pos, "d1"));
        ASSERT_CONTAINS(moves, move(pos, "e1"));
        ASSERT_CONTAINS(moves, move(pos, "f1"));
        ASSERT_CONTAINS(moves, move(pos, "g1"));
        ASSERT_EQ(moves.size(), 14);
    }
}

TEST(legal_moves_rook, rook_d4) {
    for (color c : {WHITE, BLACK}) {
        board b;
        std::string pos("d4");
        b.put_piece(ROOK, c, pos);
        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "d1"));
        ASSERT_CONTAINS(moves, move(pos, "d2"));
        ASSERT_CONTAINS(moves, move(pos, "d3"));
        ASSERT_CONTAINS(moves, move(pos, "d5"));
        ASSERT_CONTAINS(moves, move(pos, "d6"));
        ASSERT_CONTAINS(moves, move(pos, "d7"));
        ASSERT_CONTAINS(moves, move(pos, "d8"));
        ASSERT_CONTAINS(moves, move(pos, "a4"));
        ASSERT_CONTAINS(moves, move(pos, "b4"));
        ASSERT_CONTAINS(moves, move(pos, "c4"));
        ASSERT_CONTAINS(moves, move(pos, "e4"));
        ASSERT_CONTAINS(moves, move(pos, "f4"));
        ASSERT_CONTAINS(moves, move(pos, "g4"));
        ASSERT_CONTAINS(moves, move(pos, "h4"));
        ASSERT_EQ(moves.size(), 14);
    }
}

TEST(legal_moves_king, king_wont_move_into_check) {
    for (color c : {WHITE, BLACK}) {
        board b;
        auto pos = "d3";
        b.set_king_position(c, pos);
        b.put_piece(ROOK, opposite(c), "e8");

        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "d2"));
        ASSERT_CONTAINS(moves, move(pos, "c2"));
        ASSERT_CONTAINS(moves, move(pos, "c3"));
        ASSERT_CONTAINS(moves, move(pos, "c4"));
        ASSERT_CONTAINS(moves, move(pos, "d4"));
        ASSERT_NOT_CONTAINS(moves, move(pos, "e2"));
        ASSERT_NOT_CONTAINS(moves, move(pos, "e3"));
        ASSERT_NOT_CONTAINS(moves, move(pos, "e4"));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_knight, knight_at_b8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        auto pos = "b8";
        b.put_piece(KNIGHT, c, pos);

        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a6"));
        ASSERT_CONTAINS(moves, move(pos, "c6"));
        ASSERT_CONTAINS(moves, move(pos, "d7"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_knight, knight_at_b8_with_d7_obstructed) {
    for (color c : {WHITE, BLACK}) {
        board b;
        auto pos = "b8";
        b.put_piece(KNIGHT, c, pos);
        b.put_piece(PAWN, c, "d7");

        auto moves = b.get_legal_moves(c);
        list_moves(moves);
        ASSERT_CONTAINS(moves, move(pos, "a6"));
        ASSERT_CONTAINS(moves, move(pos, "c6"));
        ASSERT_NOT_CONTAINS(moves, move(pos, "d7"));
        ASSERT_EQ(moves.size(), 4 + 2 * (c == WHITE));
    }
}

TEST(board_test, rook_pinned_by_rook_from_above) {
    for (color c : {WHITE, BLACK}) {
        board b;
        auto king = "e1";
        auto rook = "e2";
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, rook);
        b.put_piece(ROOK, opposite(c), "e7");

        auto moves = b.get_legal_moves(c);
        list_moves(moves);

        ASSERT_NOT_CONTAINS(moves, move(rook, "a2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "b2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "c2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "d2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "f2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "g2"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "h2"));

        ASSERT_NOT_CONTAINS(moves, move(rook, "e1"));
        ASSERT_NOT_CONTAINS(moves, move(rook, "e8"));
        ASSERT_CONTAINS(moves, move(rook, "e3"));
        ASSERT_CONTAINS(moves, move(rook, "e4"));
        ASSERT_CONTAINS(moves, move(rook, "e5"));
        ASSERT_CONTAINS(moves, move(rook, "e6"));
        ASSERT_CONTAINS(moves, move(rook, "e7"));
        ASSERT_CONTAINS(moves, move(king, "d1"));
        ASSERT_CONTAINS(moves, move(king, "d2"));
        ASSERT_CONTAINS(moves, move(king, "f1"));
        ASSERT_CONTAINS(moves, move(king, "f2"));
        ASSERT_EQ(moves.size(), 9);
    }
}

TEST(not_under_check, knight_on_a3) {
    board b;
    b.set_initial_position();
    b.print();
    //b.make_move({"b1", "a3"});
    ASSERT_FALSE(b.under_check(WHITE));
}

TEST(legal_moves, at_initial_position) {

    board b;
    b.set_initial_position();

    auto moves = b.get_legal_moves(WHITE);

    list_moves(moves);
    ASSERT_CONTAINS(moves, move("a2", "a3"));
    ASSERT_CONTAINS(moves, move("b2", "b3"));
    ASSERT_CONTAINS(moves, move("c2", "c3"));
    ASSERT_CONTAINS(moves, move("d2", "d3"));
    ASSERT_CONTAINS(moves, move("e2", "e3"));
    ASSERT_CONTAINS(moves, move("f2", "f3"));
    ASSERT_CONTAINS(moves, move("g2", "g3"));
    ASSERT_CONTAINS(moves, move("h2", "h3"));

    ASSERT_CONTAINS(moves, move("a2", "a4"));
    ASSERT_CONTAINS(moves, move("b2", "b4"));
    ASSERT_CONTAINS(moves, move("c2", "c4"));
    ASSERT_CONTAINS(moves, move("d2", "d4"));
    ASSERT_CONTAINS(moves, move("e2", "e4"));
    ASSERT_CONTAINS(moves, move("f2", "f4"));
    ASSERT_CONTAINS(moves, move("g2", "g4"));
    ASSERT_CONTAINS(moves, move("h2", "h4"));

    ASSERT_CONTAINS(moves, move("b1", "a3"));
    ASSERT_CONTAINS(moves, move("b1", "c3"));
    ASSERT_CONTAINS(moves, move("g1", "f3"));
    ASSERT_CONTAINS(moves, move("g1", "h3"));

    b.make_move(move("e2", "e4"));

    moves = b.get_legal_moves(BLACK);

    list_moves(moves);
    ASSERT_CONTAINS(moves, move("a7", "a6"));
    ASSERT_CONTAINS(moves, move("b7", "b6"));
    ASSERT_CONTAINS(moves, move("c7", "c6"));
    ASSERT_CONTAINS(moves, move("d7", "d6"));
    ASSERT_CONTAINS(moves, move("e7", "e6"));
    ASSERT_CONTAINS(moves, move("f7", "f6"));
    ASSERT_CONTAINS(moves, move("g7", "g6"));
    ASSERT_CONTAINS(moves, move("h7", "h6"));

    ASSERT_CONTAINS(moves, move("a7", "a5"));
    ASSERT_CONTAINS(moves, move("b7", "b5"));
    ASSERT_CONTAINS(moves, move("c7", "c5"));
    ASSERT_CONTAINS(moves, move("d7", "d5"));
    ASSERT_CONTAINS(moves, move("e7", "e5"));
    ASSERT_CONTAINS(moves, move("f7", "f5"));
    ASSERT_CONTAINS(moves, move("g7", "g5"));
    ASSERT_CONTAINS(moves, move("h7", "h5"));

    ASSERT_CONTAINS(moves, move("b8", "a6"));
    ASSERT_CONTAINS(moves, move("b8", "c6"));
    ASSERT_CONTAINS(moves, move("g8", "f6"));
    ASSERT_CONTAINS(moves, move("g8", "h6"));
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
    b.put_piece(ROOK, WHITE, "d2");
    b.put_piece(KNIGHT, WHITE, "e2");
    b.set_king_position(WHITE, "f2");
    b.put_piece(PAWN, WHITE, "h2");

    b.put_piece(PAWN, WHITE, "g3");
    b.put_piece(BISHOP, WHITE, "f3");

    b.put_piece(KNIGHT, BLACK, "b5");
    b.put_piece(ROOK, BLACK, "c5");
    b.put_piece(PAWN, BLACK, "f5");

    b.put_piece(PAWN, WHITE, "c6");
    b.put_piece(BISHOP, BLACK, "e6");

    b.set_king_position(BLACK, "e7");
    b.put_piece(PAWN, BLACK, "f7");
    b.put_piece(PAWN, BLACK, "h7");

    b.print();
    ASSERT_FALSE(b.under_check(WHITE));
}
//TEST(board_test, rook_pinned_by_rook) {
//    std::random_device rd;  //Will be used to obtain a seed for the random number engine
//    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
//    std::uniform_int_distribution<> dis(1, 1000);
//
//    int k = 0;
//    for (auto kw = bitboard(1); !kw.isEmpty(); kw <<= 1) {
//        //std::cout << kw.get_square().to_string() << std::endl;
//        for (auto kb = bitboard(1); !kb.isEmpty(); kb <<= 1) {
//            if (kw == kb) continue;
//            if (std::abs(kw.get_square().get_file() - kb.get_square().get_file()) <= 1 &&
//                std::abs(kw.get_square().get_rank() - kb.get_square().get_rank()) <= 1) continue;
//
//            for (auto rw = bitboard(1); !rw.isEmpty(); rw <<= 1) {
//                if (rw == kw) continue;
//                if (rw == kb) continue;
//                for (auto rb = bitboard(1); !rb.isEmpty(); rb <<= 1) {
//                    if (rb == rw) continue;
//                    if (rb == kw) continue;
//                    if (rb == kb) continue;
//
//                    if (dis(gen) != 50) continue;
//
//                    k++;
//                    board b;
//                    b.set_king_position(WHITE, kw.get_square());
//                    b.set_king_position(BLACK, kb.get_square());
//                    b.put_piece(ROOK, WHITE, rw.get_square());
//                    b.put_piece(ROOK, BLACK, rb.get_square());
//
//                    b.calculate_attacks();
//
//                }
//            }
//        }
//    }
//    std::cout << "Tested " << k << " cases.\n";
//}

TEST(board_test, print_sizeof_board) {
    std::cout << sizeof(board) << std::endl;
}