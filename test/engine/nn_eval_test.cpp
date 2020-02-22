//
// Created by leon on 2019-08-19.
//

#include <gtest/gtest.h>
#include <chess/square.h>
#include <chess/engine/engine.h>
#include <chess/engine/nn_eval.h>
#include <chess/fen.h>

using namespace chess::core;

TEST(evaluator_test, test_single_matrix) {
    board b;
    b.set_king_position(WHITE, SQ_E1);
    b.set_king_position(BLACK, SQ_E8);
    b.put_piece(PAWN, WHITE, SQ_A2);

    std::stringstream ss;
    ss << "1\n";
    ss << "1 " << (nn_eval::INPUT_SIZE + 1) << std::endl;
    ss << "0 5";
    for (int i = 0; i < nn_eval::INPUT_SIZE - 1; i++) ss << " 0";

    nn_eval e(ss);
    int evaluation = e.eval(b);

    // EXPECT_EQ(evaluation, 5);
}

TEST(evaluator_test, two_matrices) {
    std::srand(0);
    board b;
    b.set_initial_position();

    std::stringstream ss;
    ss << "2\n";
    ss << "5 " << (nn_eval::INPUT_SIZE + 1) << std::endl;
    ss << Eigen::MatrixXd::Random(5, nn_eval::INPUT_SIZE + 1);
    ss << "\n1 6\n";
    ss << Eigen::MatrixXd::Random(1, 6) * 0.1;
    std::cout << ss.str() << "\n\n=============\n";
    nn_eval e(ss);
    int evaluation = e.eval(b);

    //EXPECT_EQ(evaluation, 948);
}

TEST(evaluator_test, three_matrices) {
    std::srand(0);
    board b;
    b.set_initial_position();

    std::stringstream ss;
    ss << "3\n";
    ss << "20 " << nn_eval::INPUT_SIZE + 1 << std::endl;
    ss << Eigen::MatrixXd::Random(20, nn_eval::INPUT_SIZE + 1);
    ss << "\n10 21\n";
    ss << Eigen::MatrixXd::Random(10, 21);
    ss << "\n1 11\n";
    ss << Eigen::MatrixXd::Random(1, 11) * .1;
    nn_eval e(ss);
    int evaluation = e.eval(b);

    //EXPECT_EQ(evaluation, -2590);
}

TEST(nn_eval_test, static_eval_should_be_simmetric) {
    auto b = fen::board_from_fen("kq6/p7/8/7N/8/8/PP6/4K2R w K--- - 0 1");
    chess::neural::mlp net(0, {nn_eval::INPUT_SIZE, 10, 1});
    nn_eval e(net);
    ASSERT_EQ(e.eval(b), -e.eval(b.flip_colors()));
}

TEST(nn_eval_test, test_fill_input_vector) {
    board b;
    b.set_initial_position();
    chess::neural::mlp net(0, {nn_eval::INPUT_SIZE, 1});
    nn_eval e(net);

    e.fill_input_vector(b);

    board b2 = e.board_from_iv(e.input_vector);

    ASSERT_EQ(b, b2);
}

TEST(nn_eval_test, test_fill_input_vector2) {
    auto b = fen::board_from_fen("kq6/p7/8/7N/8/8/PP6/4K2R b K--- - 0 1").flip_colors();
    chess::neural::mlp net(0, {nn_eval::INPUT_SIZE, 1});
    nn_eval e(net);

    e.fill_input_vector(b);

    board b2 = e.board_from_iv(e.input_vector);

    ASSERT_EQ(b, b2);
}


TEST(nn_eval_test, nn_eval_should_behave_similar_to_static) {
    std::stringstream ss;
    ss << "1\n";
    ss << "1 " << (nn_eval::INPUT_SIZE + 1) << std::endl;
    ss << "0 102 102 102 104 104 102 102 102 104 104 104 106 106 104 104 104 106 106 106 108 108 106 106 106 108 108 108 110 110 108 108 108 110 110 110 112 112 110 110 110 112 112 112 114 114 112 112 112 "
        << " 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 300 300 300 300 295 295 295 295 300 305 305 300 295 295 295 295 300 305 305 300 295 295 295 295 300 300 300 300 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 295 "
        << " 315 315 315 315 315 315 315 315 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 325 "
        << " 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 500 "
        << " 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 "
        <<  "  0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0  "
        << "-112 -112 -112 -112 -112 -112 -112 -112 -110 -110 -110 -110 -110 -110 -110 -110 -108 -108 -108 -108 -108 -108 -108 -108 -106 -106 -106 -106 -106 -106 -106 -106 -104 -104 -104 -104 -104 -104 -104 -104 -102 -102 -102 -102 -102 -102 -102 -102 "
        << "-295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -300 -300 -300 -300 -295 -295 -295 -295 -300 -305 -305 -300 -295 -295 -295 -295 -300 -305 -305 -300 -295 -295 -295 -295 -300 -300 -300 -300 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 -295 "
        << "-325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -325 -315 -315 -315 -315 -315 -315 -315 -315 "
        << "-500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 -500 "
        << "-900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 -900 "
        << "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        << "0 0 0 0 0 0 0 0 0 0 0 0\n";

    board b;
    b.set_king_position(WHITE, SQ_E1);
    b.set_king_position(BLACK, SQ_E8);
    b.put_piece(piece::QUEEN, WHITE, SQ_C4);
    nn_eval e(ss);
    nn_eval e2(chess::neural::mlp({nn_eval::INPUT_SIZE, 1}, e.net.to_eigen_vector() / 10000.0));
    int evaluation = e2.eval(b);
    b.print();
    std::cout << evaluation << std::endl;
    b.flip_colors().print();
    std::cout << e2.eval(b.flip_colors()) << std::endl;
}