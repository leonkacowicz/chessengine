//
// Created by leon on 2019-11-07.
//

#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>

#include <neuralnet.h>
#include <iostream>
#include "algorithm.h"
#include "message_queue.h"

using namespace chess::optimizer;
using chess::neural::neuralnet;

std::vector<int> algorithm::random_permutation(int size) {
    std::vector<int> indices;
    indices.reserve(size);
    for (int i = 0; i < size; i++) indices.push_back(i);
    std::shuffle(indices.begin(), indices.end(), rd);
    return indices;
}

algorithm::generation& algorithm::add_generation() {
    generation& gen = generations.emplace_back();
    gen.reserve(population_size);
    auto indices = random_permutation(population_size);
    for (int i = 0; i < population_size; i++) {
        gen.push_back({-1, -1, indices[i]});
    }
    return gen;
}

algorithm::algorithm(int num_generations, int population_size):
        num_generations(num_generations),
        population_size(population_size),
        mq(message_queue("https://sqs.us-west-2.amazonaws.com/284217563291/games.fifo", "https://sqs.us-west-2.amazonaws.com/284217563291/game_results")) {
    generations.reserve(num_generations);
    generation& gen0 = add_generation();
}

void algorithm::run() {
    for (int i = 0; i < population_size; i++) {
        player p = generate_random_player();
        players.push_back(p);
        generations[0][i].id = i;
    }

    for (int i = 0; i < population_size - 1; i += 2) {
        cross_over(generations[0][i], generations[0][i + 1]);

        request_message rm;
        mq.send_message(rm);
        //enqueue_game(generations[0][i].id, generations[0][i].child_id)
        //enqueue_game(generations[0][i + 1].id, generations[0][i + 1].child_id)
    }

    while (generations.size() < num_generations) {
        // game_result r = get_next_game_result()
        //while (r.generation + 1 >= generations.size())
            add_generation();
        // int destination = generations[r.generation][r.id].destination
        // if (r.parent_won()) generations[r.generation + 1][destination].id = generations[r.generation][r.id].id;
        // else generations[r.generation + 1][destination].id = generations[r.generation][r.id].child_id;

        // if exists generations[r.generation + 1] for indices 2 * (destination >> 1) and 2 * (destination >> 1) + 1,
        //      perform cross-over between them;
        //      enqueue games
    }
}

algorithm::player algorithm::generate_random_player() {
    return save_player(neuralnet(std::move(rd), {832, 10, 1}));
}

void algorithm::cross_over(algorithm::parent& p1, algorithm::parent& p2) {
    int theta_size = players[p1.id].theta.size();
    const Eigen::VectorXd& theta1 = players[p1.id].theta;
    const Eigen::VectorXd& theta2 = players[p2.id].theta;
    auto theta1_child = theta1;
    auto theta2_child = theta2;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dis(0, 1);

    for (int i = 0; i < theta_size; i++) {
        if (dis(mt) == 0) theta1_child(i) = theta1(i);
        else theta1_child(i) = theta2(i);
        if (dis(mt) == 0) theta2_child(i) = theta1(i);
        else theta2_child(i) = theta2(i);
    }

    if ((theta1_child - theta1).squaredNorm() + (theta2_child - theta2).squaredNorm() >
        (theta1_child - theta2).squaredNorm() + (theta2_child - theta1).squaredNorm()) {
        for (int i = 0; i < theta_size; i++) {
            double tmp = theta1_child(i);
            theta1_child(i) = theta2_child(i);
            theta2_child(i) = tmp;
        }
    }

    auto child1 = save_player(theta1_child);
    auto child2 = save_player(theta2_child);

    players.push_back(child1);
    p1.child_id = players.size();
    players.push_back(child2);
    p2.child_id = players.size();
}

algorithm::player algorithm::save_player(const Eigen::VectorXd& theta) {
    return save_player(neuralnet({832, 10, 1}, theta));
}

algorithm::player algorithm::save_player(const neuralnet& nn) {
    algorithm::player p;
    p.theta = nn.to_eigen_vector();
    boost::filesystem::path file = boost::filesystem::unique_path();
    nn.output_to_stream(std::ofstream(file.string()));
    boost::process::ipstream out;
    boost::process::system("sha256sum " + file.string(), boost::process::std_out > out);
    out >> p.hash;
    boost::process::system(boost::process::search_path("aws"), "s3", "cp", file.string(), "s3://leonkacowicz/chess/players/" + p.hash + ".txt");
    boost::filesystem::remove(file);
    return p;
}

algorithm::game_result algorithm::get_next_game_result() {
    using namespace std::chrono_literals;
    response_message msg;
    while (!mq.receive_message(msg, 10s)) std::cout << "Waiting for game results";

    assert(msg.generation < generations.size());
    assert(msg.array_index < population_size);
    assert(generations[msg.generation][msg.array_index].id == msg.id);

    //if (generations[msg.generation][msg.id].)

//    game_result gr{
//        .id = msg.id, .generation = msg.generation
//    };
    return algorithm::game_result();
}
