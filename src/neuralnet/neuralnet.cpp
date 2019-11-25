#include <fstream>
#include "neuralnet.h"

using namespace chess::neural;

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

neuralnet::neuralnet(const std::vector<int>& layer_sizes, const Eigen::VectorXd& theta) {
    int k = 0;
    for (int layer = 0; layer < layer_sizes.size() - 1; layer++) {
        int rows = layer_sizes[layer + 1], cols = layer_sizes[layer] + 1;
        matrices.emplace_back(rows, cols);
        auto& M = matrices.back();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                M(i, j) = theta(k++);
    }
}

Eigen::VectorXd neuralnet::operator()(const Eigen::VectorXd& input_vector) {
    Eigen::VectorXd v(input_vector);
    for (auto& M : matrices) {
        auto size = M.cols();
        Eigen::VectorXd w(size);
        w << 1, v;
        v = M * w;
        for (int j = 0; j < v.size(); j++) v[j] = 1 / (1 + std::exp(-v[j]));
    }
    return v;
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

Eigen::VectorXd neuralnet::to_eigen_vector() const {

    int theta_size = 0;
    for (auto& M : matrices) theta_size += M.rows() * M.cols();
    Eigen::VectorXd ret(theta_size);
    int k = 0;
    for (auto& M : matrices) {
        int rows = M.rows(), cols = M.cols();
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                ret(k++) = M(i, j);
    }
    return ret;
}

neuralnet::neuralnet(std::random_device&& rd, const std::vector<int>& layers) {
    std::mt19937 mt(rd());
    std::normal_distribution dis(0.0, 1.0);
    int num_layers = layers.size();
    matrices.reserve(num_layers);
    for (int i = 1; i < num_layers; i++) {
        int rows = layers[i], cols = layers[i - 1] + 1;
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(rows, cols);
        for (int row = 0; row < rows; row++) for (int col = 0; col < cols; col++) M(row, col) = .1 * dis(mt) / dis(mt);
        matrices.push_back(M);
    }
}
