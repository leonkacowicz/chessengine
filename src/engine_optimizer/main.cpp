//
// Created by leon on 2019-08-18.
//

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Eigen>
#include <iostream>
#include <fstream>
#include <boost/process.hpp>

struct neuralnet {
    std::vector<Eigen::MatrixXd> matrices;
};

int main() {
    auto M = Eigen::MatrixXf::Random(2, 3);


//    std::ifstream file("test.txt");
//
//    for (int i = 0; i < 2; i++)
//        for (int j = 0; j < 3; j++)
//            file >> M(i, j);
//    file.close();

    Eigen::VectorXf v {{ 1, 2, 3 }} ;

    Eigen::VectorXf w(3);

    w << M * v, 1;

    std::cout << M << std::endl;
    std::cout << v << std::endl;
    std::cout << M * v << std::endl;
    std::cout << w << std::endl;
    Eigen::ArrayXf ones(3);
    ones << 1, 1, 1;
    std::cout << (w.array() < 1).matrix().cast<double>() * 5.1 << std::endl;

    boost::process::ipstream pipe_stream;
    boost::process::opstream input;
    boost::process::child ls("cat -n", boost::process::std_out > pipe_stream, boost::process::std_in < input);

    input << "hello world\nhello!!";
    input.flush();
    input.close();
    input.pipe().close();
    std::string line;
//    pipe_stream >> line;
    while (pipe_stream.good() && std::getline(pipe_stream, line) && !line.empty()) {
        std::cout << line << std::endl;
    }
//    std::cout << line << std::endl;
    ls.wait();
    std::cout << "finished\n";
}