#ifndef CHESSENGINE_MLP_H
#define CHESSENGINE_MLP_H

#include <vector>
#include <Eigen/Core>
#include <random>

namespace chess::neural {

class mlp {
    std::vector<Eigen::MatrixXd> matrices;
public:
    mlp(std::istream&);
    mlp(uint64_t seed, const std::vector<int>& layers);
    mlp(const std::vector<Eigen::MatrixXd>&);
    mlp(const std::vector<int>&, const std::vector<double>&);
    mlp(const std::vector<int>&, const Eigen::VectorXd&);
    Eigen::VectorXd operator()(const Eigen::VectorXd&) const;
    void output_to_stream(std::ostream&& os) const;
    std::vector<double> to_vector() const;
    Eigen::VectorXd to_eigen_vector() const;
    static int num_vector_dimensions(const std::vector<int>& layers);
};

}


#endif //CHESSENGINE_MLP_H
