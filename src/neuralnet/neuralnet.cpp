#include <fstream>
#include "neuralnet.h"

namespace chess { namespace neural {

neuralnet::neuralnet(std::istream& fin) {
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

neuralnet::neuralnet(const std::vector<Eigen::MatrixXd>& matrices) : matrices(matrices) {

}

neuralnet::neuralnet(const std::vector<int>& layer_sizes, const std::vector<double>& theta) {
    int k = 0;
    for (int layer = 0; layer < layer_sizes.size() - 1; layer++) {
        int rows = layer_sizes[layer + 1], cols = layer_sizes[layer] + 1;
        matrices.emplace_back(rows, cols);
        auto& M = matrices.back();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                M(i, j) = theta[k++];
    }
}

Eigen::VectorXd neuralnet::operator()(const Eigen::VectorXd& input_vector) {
    Eigen::VectorXd v(input_vector.size() + 1);
    v << 1, input_vector;

    for (int i = 0; i < matrices.size() - 1; i++) {
        Eigen::VectorXd w(matrices[i].rows() + 1);
        w << 1, matrices[i] * v;
        v = (w.array() > 0).matrix().cast<double>();
    }

    return matrices.back() * v;
}

void neuralnet::output_to_stream(std::ostream&& os) const {
    os << matrices.size() << std::endl;
    for (auto& matrix : matrices) {
        os << matrix.rows() << " " << matrix.cols() << std::endl;
        os << matrix << std::endl;
    }
}

std::vector<double> neuralnet::to_vector() const {

    int theta_size = 0;
    for (auto& M : matrices) theta_size += M.rows() * M.cols();
    std::vector<double> ret;
    ret.reserve(theta_size);
    for (auto& M : matrices) {
        int rows = M.rows(), cols = M.cols();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                ret.push_back(M(i, j));
    }
    return ret;
}

}}
