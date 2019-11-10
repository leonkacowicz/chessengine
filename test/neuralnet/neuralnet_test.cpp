#include <gtest/gtest.h>
#include <neuralnet.h>

TEST(neuralnet, neuralnet_happycase_test) {
    std::srand(0);
    constexpr int input_size = 832;
    std::stringstream in;
    std::vector<int> layers;

    in << "3" << std::endl;
    in << "6 " << (input_size + 1) << std::endl;
    in << Eigen::MatrixXd::Random(6, input_size + 1) << std::endl;
    in << "4 7" << std::endl;
    in << Eigen::MatrixXd::Random(4, 7) << std::endl;
    in << "2 5" << std::endl;
    in << Eigen::MatrixXd::Random(2, 5) << std::endl;

    chess::neural::neuralnet n(in);

    auto v = Eigen::VectorXd::Random(input_size);

    auto y = n(v);
    ASSERT_DOUBLE_EQ(y[0], 0.0580645);
    ASSERT_DOUBLE_EQ(y[1], 0.055746);
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
    ASSERT_DOUBLE_EQ(y[0], 0.058064354144997998);
    ASSERT_DOUBLE_EQ(y[1], 0.055746152557314521);
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

    ASSERT_DOUBLE_EQ(y[0], 0.058064354144997998);
    ASSERT_DOUBLE_EQ(y[1], 0.055746152557314521);
}