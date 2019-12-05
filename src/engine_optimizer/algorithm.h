//
// Created by leon on 2019-11-07.
//

#ifndef CHESSENGINE_ALGORITHM_H
#define CHESSENGINE_ALGORITHM_H

#include <unordered_map>
#include <Eigen/Core>
#include <random>
#include <neuralnet.h>
#include "message_queue.h"

namespace chess::optimizer {
    class algorithm {

        struct parent {
            int id = -1;
            int child_id = -1;
            int destination = -1;
        };

        struct player {
            std::string hash;
            Eigen::VectorXd theta;
        };

        typedef std::vector<parent> generation;
        std::vector<generation> generations;
        std::vector<player> players;
        int num_generations;
        int population_size;

        std::random_device rd;

        std::vector<int> random_permutation(int size);
        generation& add_generation();
        player generate_random_player();
        void cross_over(parent& p1, parent& p2);
        player save_player(const Eigen::VectorXd& theta);
        player save_player(const chess::neural::neuralnet& nn);

        struct game_result {
            int generation;
            int array_index;
            int id;
            int winner_id;
        };

        message_queue mq;
        game_result get_next_game_result();
    public:
        algorithm(int num_generations, int population_size);
        void run();
    };
};

#endif //CHESSENGINE_ALGORITHM_H
