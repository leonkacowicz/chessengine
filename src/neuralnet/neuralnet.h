#ifndef CHESSENGINE_NEURALNET_H
#define CHESSENGINE_NEURALNET_H

#include <vector>
#include <Eigen/Core>

namespace chess {
    namespace neural {
        class neuralnet {
            std::vector<Eigen::MatrixXd> matrices;
        public:
            neuralnet(std::istream&);
            neuralnet(const std::vector<Eigen::MatrixXd>&);
            Eigen::VectorXd operator()(const Eigen::VectorXd&);
            void output_to_stream(std::ostream&& os) const;
        };

    }
}


#endif //CHESSENGINE_NEURALNET_H
