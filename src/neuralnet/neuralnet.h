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
            Eigen::VectorXd operator()(const Eigen::VectorXd&);
        };
    }
}


#endif //CHESSENGINE_NEURALNET_H
