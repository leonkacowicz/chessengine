//
// Created by leon on 2020-01-22.
//


#include <iostream>
#include <fstream>
#include <Eigen/Core>
#include "supervised_trainer.h"

int main() {

    std::ifstream ifs("out.txt");
    int k = 0;
    chess::optimizer::supervised_trainer sto({880, 1});
    std::string line;
    while (std::getline(ifs, line, '\n')) {
        if (line.empty()) break;
        if (++k >= 10) break;
        std::stringstream ss(line);
        assert(!line.empty());
        double y;
        ss >> y;
        std::vector<double> v;
        {
            double x;
            while (ss >> x) {
                v.push_back(x);
            }
        }
        Eigen::VectorXd x(v.size());
        assert(v.size() == 880);
        for (int i = 0; i < v.size(); i++) x[i] = v[i];

        sto.training_set.push_back(x);
        sto.training_set_outputs.push_back((Eigen::VectorXd(1) << y).finished());
    }
    sto.train(100000);
}