#ifndef CHESSENGINE_MLP_H
#define CHESSENGINE_MLP_H

#include <vector>
#include <Eigen/Core>
#include <random>

namespace chess {
    namespace neural {
        class mlp {
            std::vector<Eigen::MatrixXd> matrices;
        public:
            mlp(std::istream&);
            mlp(std::random_device&& rd, const std::vector<int>& layers);

            mlp(const std::vector<Eigen::MatrixXd>&);
            mlp(const std::vector<int>&, const std::vector<double>&);
            mlp(const std::vector<int>&, const Eigen::VectorXd&);
            Eigen::VectorXd operator()(const Eigen::VectorXd&);
            void output_to_stream(std::ostream&& os) const;
            std::vector<double> to_vector() const;
            Eigen::VectorXd to_eigen_vector() const;
        };

    }
}


#endif //CHESSENGINE_MLP_H
