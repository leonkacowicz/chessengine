//
// Created by leon on 2019-08-19.
//

#include <gtest/gtest.h>
#include <chess/square.h>
#include <engine.h>
#include <nn_eval.h>
#include <chess/fen.h>

using namespace chess::core;

TEST(evaluator_test, test_single_matrix) {
    board b;
    b.set_king_position(WHITE, SQ_E1);
    b.set_king_position(BLACK, SQ_E8);
    b.put_piece(PAWN, WHITE, SQ_A1);

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
    std::random_device rd;
    chess::neural::mlp net(rd, {nn_eval::INPUT_SIZE, 10, 1});
    nn_eval e(net);
    ASSERT_EQ(e.eval(b), -e.eval(b.flip_colors()));
}