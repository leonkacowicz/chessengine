#include <Eigen/Core>
#include <iostream>
#include <fstream>
#include <boost/process.hpp>
#include <neuralnet.h>

using chess::neural::neuralnet;

neuralnet random_net() {
    neuralnet nn({Eigen::MatrixXd::Random(6, 833), Eigen::MatrixXd::Random(1, 7)});
    return nn;
}

int main() {
    random_net().output_to_stream(std::ofstream("test_white.txt"));
    random_net().output_to_stream(std::ofstream("test_black.txt"));

    std::ofstream white_options("white_options.txt");
    white_options << "setoption name evaluator value test_white.txt";
    white_options.close();

    std::ofstream black_options("black_options.txt");
    black_options << "setoption name evaluator value test_black.txt";
    black_options.close();

    boost::process::system("../arbiter/chessarbiter",
            "--verbose",
            "--white-exec", "../engine/chessengine",
            "--white-input", "white_options.txt",
            "--white-move-time", "5000",
            "--black-exec", "../engine/chessengine",
            "--black-input", "black_options.txt",
            "--black-move-time", "5000"
    );

}