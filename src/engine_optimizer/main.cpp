#include <Eigen/Core>
#include <iostream>
#include <fstream>
#include <boost/process.hpp>

struct neuralnet {
    std::vector<Eigen::MatrixXd> matrices;

    void save_to_file(const std::string& filename) const {
        std::ofstream ofs(filename);
        ofs << matrices.size() << std::endl;
        for (auto& matrix : matrices) {
            ofs << matrix.rows() << " " << matrix.cols() << std::endl;
            ofs << matrix << std::endl;
        }
    }
};

neuralnet random_net() {
    neuralnet nn;
    nn.matrices.push_back(Eigen::MatrixXd::Random(6, 833) * 100);
    nn.matrices.push_back(Eigen::MatrixXd::Random(1, 7));
    return nn;
}

int main() {
    random_net().save_to_file("test_white.txt");
    random_net().save_to_file("test_black.txt");

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