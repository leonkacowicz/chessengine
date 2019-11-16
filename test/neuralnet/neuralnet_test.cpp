#include <gtest/gtest.h>
#include <neuralnet.h>

TEST(neuralnet, neuralnet_happycase_test) {
    std::srand(0);
    constexpr int input_size = 832;
    std::stringstream in;
    std::vector<int> layers;

    auto M1 = Eigen::MatrixXd::Random(6, input_size + 1);
    auto M2 = Eigen::MatrixXd::Random(4, 7);
    auto M3 = Eigen::MatrixXd::Random(2, 5);
    auto v = Eigen::VectorXd::Random(input_size);

    in << "3" << std::endl;
    in << M1.rows() << " " << M1.cols() << std::endl;
    in << M1 << std::endl;
    in << M2.rows() << " " << M2.cols() << std::endl;
    in << M2 << std::endl;
    in << M3.rows() << " " << M3.cols() << std::endl;
    in << M3 << std::endl;

    chess::neural::neuralnet n(in);

    auto y = n(v);
    ASSERT_DOUBLE_EQ(y[0], -1.1361272071026871);
    ASSERT_DOUBLE_EQ(y[1], 11.493079511952136);
}

TEST(neuralnet, neuralnet_from_matrices) {
    std::srand(0);
    constexpr int input_size = 832;
    std::vector<Eigen::MatrixXd> matrices;
    matrices.push_back(Eigen::MatrixXd::Random(6, input_size + 1));
    matrices.push_back(Eigen::MatrixXd::Random(4, 7));
    matrices.push_back(Eigen::MatrixXd::Random(2, 5));

    chess::neural::neuralnet n(matrices);

    auto v = Eigen::VectorXd::Random(input_size);

    auto y = n(v);
    ASSERT_DOUBLE_EQ(y[0], -1.1361282966429407);
    ASSERT_DOUBLE_EQ(y[1], 11.493079379636622);
}

TEST(neuralnet, to_vector_from_vector) {
    std::srand(0);
    constexpr int input_size = 832;
    std::vector<Eigen::MatrixXd> matrices;
    matrices.push_back(Eigen::MatrixXd::Random(6, input_size + 1));
    matrices.push_back(Eigen::MatrixXd::Random(4, 7));
    matrices.push_back(Eigen::MatrixXd::Random(2, 5));

    chess::neural::neuralnet n(matrices);

    auto v = Eigen::VectorXd::Random(input_size);

    chess::neural::neuralnet n2({832, 6, 4, 2}, n.to_vector());

    auto y = n2(v);

    ASSERT_DOUBLE_EQ(y[0], -1.1361282966429407);
    ASSERT_DOUBLE_EQ(y[1], 11.493079379636622);
}