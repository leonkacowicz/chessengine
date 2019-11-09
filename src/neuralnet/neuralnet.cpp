#include <fstream>
#include "neuralnet.h"

chess::neural::neuralnet::neuralnet(std::istream& fin) {
    int num_layers;
    fin >> num_layers;
    while (num_layers--) {
        int rows, cols;
        fin >> rows >> cols;
        matrices.emplace_back(rows, cols);
        auto& M = matrices.back();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                fin >> M(i, j);
    }
}

Eigen::VectorXd chess::neural::neuralnet::operator()(const Eigen::VectorXd& input_vector) {
    Eigen::VectorXd v(input_vector.size() + 1);
    v << 1, input_vector;

    for (int i = 0; i < matrices.size() - 1; i++) {
        Eigen::VectorXd w(matrices[i].rows() + 1);
        w << 1, matrices[i] * v;
        v = (w.array() > 0).matrix().cast<double>();
    }

    return matrices.back() * v;
}
