//
// Created by leon on 2019-08-10.
//

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <chess/board.h>
#include <chess/square.h>
#include <chess/fen.h>
#include <chess/move_gen.h>
#include "../test_common.h"

using namespace chess::core;

TEST(move_gen_test, king_move_into_check_by_pawn_1) {
    board b = fen::board_from_fen("Nn4q1/7r/3B1k1b/p3pP2/N4P2/2PKRn1P/b3B3/3Q3R b - - 1 52");
    auto moves = move_gen(b).generate();
    ASSERT_NOT_CONTAINS(moves, get_move(SQ_F6, SQ_G5));
}


TEST(legal_moves, at_initial_position) {

    board b;
    b.set_initial_position();

    auto moves = move_gen(b).generate();

    ASSERT_CONTAINS(moves, get_move("a2", "a3"));
    ASSERT_CONTAINS(moves, get_move("b2", "b3"));
    ASSERT_CONTAINS(moves, get_move("c2", "c3"));
    ASSERT_CONTAINS(moves, get_move("d2", "d3"));
    ASSERT_CONTAINS(moves, get_move("e2", "e3"));
    ASSERT_CONTAINS(moves, get_move("f2", "f3"));
    ASSERT_CONTAINS(moves, get_move("g2", "g3"));
    ASSERT_CONTAINS(moves, get_move("h2", "h3"));

    ASSERT_CONTAINS(moves, get_move("a2", "a4"));
    ASSERT_CONTAINS(moves, get_move("b2", "b4"));
    ASSERT_CONTAINS(moves, get_move("c2", "c4"));
    ASSERT_CONTAINS(moves, get_move("d2", "d4"));
    ASSERT_CONTAINS(moves, get_move("e2", "e4"));
    ASSERT_CONTAINS(moves, get_move("f2", "f4"));
    ASSERT_CONTAINS(moves, get_move("g2", "g4"));
    ASSERT_CONTAINS(moves, get_move("h2", "h4"));

    ASSERT_CONTAINS(moves, get_move("b1", "a3"));
    ASSERT_CONTAINS(moves, get_move("b1", "c3"));
    ASSERT_CONTAINS(moves, get_move("g1", "f3"));
    ASSERT_CONTAINS(moves, get_move("g1", "h3"));

    b.make_move(get_move("e2", "e4"));

    moves = move_gen(b).generate();

    ASSERT_CONTAINS(moves, get_move("a7", "a6"));
    ASSERT_CONTAINS(moves, get_move("b7", "b6"));
    ASSERT_CONTAINS(moves, get_move("c7", "c6"));
    ASSERT_CONTAINS(moves, get_move("d7", "d6"));
    ASSERT_CONTAINS(moves, get_move("e7", "e6"));
    ASSERT_CONTAINS(moves, get_move("f7", "f6"));
    ASSERT_CONTAINS(moves, get_move("g7", "g6"));
    ASSERT_CONTAINS(moves, get_move("h7", "h6"));

    ASSERT_CONTAINS(moves, get_move("a7", "a5"));
    ASSERT_CONTAINS(moves, get_move("b7", "b5"));
    ASSERT_CONTAINS(moves, get_move("c7", "c5"));
    ASSERT_CONTAINS(moves, get_move("d7", "d5"));
    ASSERT_CONTAINS(moves, get_move("e7", "e5"));
    ASSERT_CONTAINS(moves, get_move("f7", "f5"));
    ASSERT_CONTAINS(moves, get_move("g7", "g5"));
    ASSERT_CONTAINS(moves, get_move("h7", "h5"));

    ASSERT_CONTAINS(moves, get_move("b8", "a6"));
    ASSERT_CONTAINS(moves, get_move("b8", "c6"));
    ASSERT_CONTAINS(moves, get_move("g8", "f6"));
    ASSERT_CONTAINS(moves, get_move("g8", "h6"));
}

TEST(board_test, _1b4n1_p2b4_1k3pR1_3P1n1p_1PB2P1P_8_PB2P1N1_RN2K3__w__Q_____1__51) {

    // play 51 should not be legal:
    // 1. Nf3  d5 2. Na3  Be6 3. h4  Qd7 4. b4  Bf5 5. Ng1  Qc6 6. c3  Qd7 7. Nb1  g5
    // 8. c4  Kd8 9. Qa4  Qc8 10. Qb3  Qd7 11. Qh3  Qe8 12. g3  Bg7 13. cxd5  g4 14. Qxg4  c6 15. Qd4  Bh3 16. Qe4  Bh6
    // 17. Qc2  Bf5 18. Bg2  Bg7 19. Qe4  Bc8 20. Ba3  Kd7 21. Qd3  Kc7 22. Qb5  Bg4 23. Qxb7+  Kd6 24. dxc6  h5
    // 25. Qxa8  Qd8 26. Nf3  Qf8 27. Rg1  Qd8 28. Bh3  Bf6 29. Qb7  Bc8 30. Bf5  Nh6 31. Bd3  Bg5 32. Rg2  Ke6
    // 33. c7  Kd7 34. Qa6  Bf4 35. Qc6+  Nxc6 36. Nd4  Bb7 37. Ba6  Bxg3 38. Nc2  Bc8 39. cxd8=B  Bb8 40. Ba5  Rd8
    // 41. Rg6  Bf4 42. d4  f6 43. Bxd8  Kd6 44. Bb2  Ng8 45. Bxe7+  Nc6xe7 46. Ne3  Kc6 47. Ng2  Nf5 48. d5+  Kb6
    // 49. Bc4  Bb8 50. f4  Bd7 51. O-O

    board b = fen::board_from_fen("1b4n1/p2b4/1k3pR1/3P1n1p/1PB2P1P/8/PB2P1N1/RN2K3 w Q - 1 51");

    auto moves = move_gen(b).generate();
    for (auto& m : moves) {
        ASSERT_NE(move_type(m), CASTLE_KING_SIDE_WHITE);
    }
}

TEST(board_test, legal_moves_position_1) {
    board b = fen::board_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    b.make_move(get_move("f1", "f2"));
    auto legal_moves = move_gen(b).generate();
    int num_moves = legal_moves.size();
    ASSERT_CONTAINS(legal_moves, get_move(SQ_E8, SQ_C8, CASTLE_QUEEN_SIDE_BLACK));
    ASSERT_EQ(num_moves, 45);
}



TEST(legal_moves_king, king_first_rank) {
    for (color c : {WHITE, BLACK}) {
        board b;
        b.set_king_position(c, SQ_E1);
        b.set_king_position(opposite(c), SQ_E8);
        b.side_to_play = c;
        const std::vector<move> moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move("e1", "d1"));
        ASSERT_CONTAINS(moves, get_move("e1", "d2"));
        ASSERT_CONTAINS(moves, get_move("e1", "e2"));
        ASSERT_CONTAINS(moves, get_move("e1", "f2"));
        ASSERT_CONTAINS(moves, get_move("e1", "f1"));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_king, king_last_rank) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_E8;
        b.set_king_position(c, SQ_E8);
        b.side_to_play = c;
        const std::vector<move> moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, SQ_D8));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_D7));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_E7));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_F7));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_F8));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_king, king_h1) {
    for (color c : {WHITE, BLACK}) {
        board b;

        square pos = SQ_H1;
        b.set_king_position(c, pos);
        b.side_to_play = c;
        const std::vector<move> moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, "h2"));
        ASSERT_CONTAINS(moves, get_move(pos, "g2"));
        ASSERT_CONTAINS(moves, get_move(pos, "g1"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_a1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_A1;
        b.set_king_position(c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, "a2"));
        ASSERT_CONTAINS(moves, get_move(pos, "b2"));
        ASSERT_CONTAINS(moves, get_move(pos, "b1"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_a8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_A8;
        b.set_king_position(c, pos);
        b.side_to_play = c;

        auto moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, "a7"));
        ASSERT_CONTAINS(moves, get_move(pos, "b7"));
        ASSERT_CONTAINS(moves, get_move(pos, "b8"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_h8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_H8;
        b.set_king_position(c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, "h7"));
        ASSERT_CONTAINS(moves, get_move(pos, "g7"));
        ASSERT_CONTAINS(moves, get_move(pos, "g8"));
        ASSERT_EQ(moves.size(), 3);
    }
}

TEST(legal_moves_king, king_e5) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_E5;
        b.set_king_position(c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();

        
        ASSERT_CONTAINS(moves, get_move(pos, "d4"));
        ASSERT_CONTAINS(moves, get_move(pos, "d5"));
        ASSERT_CONTAINS(moves, get_move(pos, "d6"));
        ASSERT_CONTAINS(moves, get_move(pos, "e4"));
        ASSERT_CONTAINS(moves, get_move(pos, "e6"));
        ASSERT_CONTAINS(moves, get_move(pos, "f4"));
        ASSERT_CONTAINS(moves, get_move(pos, "f5"));
        ASSERT_CONTAINS(moves, get_move(pos, "f6"));
        ASSERT_EQ(moves.size(), 8);
    }
}

TEST(legal_moves_rook, rook_a1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square king = SQ_H8;
        square rook = SQ_A1;
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, rook);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A3));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A4));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A5));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A6));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A7));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_B1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_C1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_D1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_E1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_F1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_G1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H1));
        ASSERT_CONTAINS(moves, get_move(king, SQ_H7));
        ASSERT_CONTAINS(moves, get_move(king, SQ_G8));
        ASSERT_CONTAINS(moves, get_move(king, SQ_G7));
        ASSERT_EQ(moves.size(), 17);
    }
}

TEST(legal_moves_rook, rook_a8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square king = SQ_H1;
        square rook = SQ_A8;
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, rook);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A3));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A4));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A5));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A6));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A7));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A1));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_B8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_C8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_D8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_E8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_F8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_G8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H8));
        ASSERT_CONTAINS(moves, get_move(king, SQ_H2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_G1));
        ASSERT_CONTAINS(moves, get_move(king, SQ_G2));
        ASSERT_EQ(moves.size(), 17);
    }
}

TEST(legal_moves_rook, rook_h8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square king = SQ_A1;
        square rook = SQ_H8;
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, rook);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H2));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H3));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H4));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H5));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H6));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H7));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_A8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_B8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_C8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_D8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_E8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_F8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_G8));
        ASSERT_CONTAINS(moves, get_move(rook, SQ_H1));
        ASSERT_CONTAINS(moves, get_move(king, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B1));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B2));
        ASSERT_EQ(moves.size(), 17);
    }
}

TEST(legal_moves_rook, rook_h1) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square king = SQ_A8;
        square pos = SQ_H1;
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H2));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H3));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H4));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H5));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H6));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H7));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_H8));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_A1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_B1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_C1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_D1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_E1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_F1));
        ASSERT_CONTAINS(moves, get_move(pos, SQ_G1));
        ASSERT_CONTAINS(moves, get_move(king, SQ_A7));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B8));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B7));
        ASSERT_EQ(moves.size(), 17);
    }
}

TEST(legal_moves_rook, rook_d4) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square king = SQ_A1;
        square pos = SQ_D4;
        b.set_king_position(c, king);
        b.put_piece(ROOK, c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(pos, "d1"));
        ASSERT_CONTAINS(moves, get_move(pos, "d2"));
        ASSERT_CONTAINS(moves, get_move(pos, "d3"));
        ASSERT_CONTAINS(moves, get_move(pos, "d5"));
        ASSERT_CONTAINS(moves, get_move(pos, "d6"));
        ASSERT_CONTAINS(moves, get_move(pos, "d7"));
        ASSERT_CONTAINS(moves, get_move(pos, "d8"));
        ASSERT_CONTAINS(moves, get_move(pos, "a4"));
        ASSERT_CONTAINS(moves, get_move(pos, "b4"));
        ASSERT_CONTAINS(moves, get_move(pos, "c4"));
        ASSERT_CONTAINS(moves, get_move(pos, "e4"));
        ASSERT_CONTAINS(moves, get_move(pos, "f4"));
        ASSERT_CONTAINS(moves, get_move(pos, "g4"));
        ASSERT_CONTAINS(moves, get_move(pos, "h4"));
        ASSERT_CONTAINS(moves, get_move(king, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B1));

        ASSERT_EQ(moves.size(), 17);
    }
}

TEST(legal_moves_king, king_wont_move_into_check) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_D3;
        b.set_king_position(c, pos);
        b.set_king_position(opposite(c), SQ_A1);
        b.put_piece(ROOK, opposite(c), SQ_E8);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(pos, "d2"));
        ASSERT_CONTAINS(moves, get_move(pos, "c2"));
        ASSERT_CONTAINS(moves, get_move(pos, "c3"));
        ASSERT_CONTAINS(moves, get_move(pos, "c4"));
        ASSERT_CONTAINS(moves, get_move(pos, "d4"));
        ASSERT_NOT_CONTAINS(moves, get_move(pos, "e2"));
        ASSERT_NOT_CONTAINS(moves, get_move(pos, "e3"));
        ASSERT_NOT_CONTAINS(moves, get_move(pos, "e4"));
        ASSERT_EQ(moves.size(), 5);
    }
}

TEST(legal_moves_knight, knight_at_b8) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_B8;
        auto king = SQ_A1;
        b.set_king_position(c, king);
        b.set_king_position(opposite(c), SQ_H8);
        b.put_piece(KNIGHT, c, pos);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(pos, "a6"));
        ASSERT_CONTAINS(moves, get_move(pos, "c6"));
        ASSERT_CONTAINS(moves, get_move(pos, "d7"));
        ASSERT_CONTAINS(moves, get_move(king, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B1));
        ASSERT_EQ(moves.size(), 6);
    }
}

TEST(legal_moves_knight, knight_at_b8_with_c6_obstructed) {
    for (color c : {WHITE, BLACK}) {
        board b;
        square pos = SQ_B8;
        auto king = SQ_A1;
        b.set_king_position(c, king);
        b.set_king_position(opposite(c), SQ_H8);
        b.put_piece(KNIGHT, c, pos);
        b.put_piece(PAWN, c, SQ_C6);
        b.side_to_play = c;
        auto moves = move_gen(b).generate();
        
        ASSERT_CONTAINS(moves, get_move(pos, "a6"));
        ASSERT_CONTAINS(moves, get_move(pos, "d7"));
        ASSERT_NOT_CONTAINS(moves, get_move(pos, "c6"));
        ASSERT_CONTAINS(moves, get_move(king, SQ_A2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B2));
        ASSERT_CONTAINS(moves, get_move(king, SQ_B1));
        ASSERT_EQ(moves.size(), 6);
    }
}

TEST(board_test, rook_pinned_by_rook_from_above) {
    for (color c : {WHITE, BLACK}) {
        board b;
        auto king = SQ_E1;
        auto other_king = SQ_H8;
        auto rook = SQ_E2;
        b.set_king_position(c, king);
        b.set_king_position(opposite(c), other_king);
        b.put_piece(ROOK, c, rook);
        b.put_piece(ROOK, opposite(c), SQ_E7);
        b.side_to_play = c;
        b.print();
        auto moves = move_gen(b).generate();
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "a2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "b2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "c2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "d2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "f2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "g2"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "h2"));

        ASSERT_NOT_CONTAINS(moves, get_move(rook, "e1"));
        ASSERT_NOT_CONTAINS(moves, get_move(rook, "e8"));
        ASSERT_CONTAINS(moves, get_move(rook, "e3"));
        ASSERT_CONTAINS(moves, get_move(rook, "e4"));
        ASSERT_CONTAINS(moves, get_move(rook, "e5"));
        ASSERT_CONTAINS(moves, get_move(rook, "e6"));
        ASSERT_CONTAINS(moves, get_move(rook, "e7"));
        ASSERT_CONTAINS(moves, get_move(king, "d1"));
        ASSERT_CONTAINS(moves, get_move(king, "d2"));
        ASSERT_CONTAINS(moves, get_move(king, "f1"));
        ASSERT_CONTAINS(moves, get_move(king, "f2"));
        ASSERT_EQ(moves.size(), 9);
    }
}