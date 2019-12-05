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

algorithm::algorithm(int num_generations, int population_size) :
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
        enqueue_game(0, i, (i + 1) % 2 );
        enqueue_game(0, i + 1, i % 2 );
    }

    while (generations.size() < num_generations) {
        game_result r = get_next_game_result();
        while (r.generation + 1 >= generations.size())
            add_generation();
        int destination = generations[r.generation][r.array_index].destination;
        if (r.winner_id == generations[r.generation][r.array_index].id)
            generations[r.generation + 1][destination].id = generations[r.generation][r.id].id;
        else
            generations[r.generation + 1][destination].id = generations[r.generation][r.id].child_id;

        int base_index = destination - (destination % 2);
        if (generations[r.generation + 1][base_index].id > -1 && generations[r.generation + 1][base_index + 1].id > -1) {
            cross_over(generations[r.generation + 1][base_index], generations[r.generation + 1][base_index + 1]);
            mq.send_message({
                .white = players[generations[r.generation + 1][base_index].id].hash,
                .black = players[generations[r.generation + 1][base_index].child_id].hash,
                .outputdir = "chess/generations/",
                .generation = r.generation + 1,
                .id = generations[r.generation + 1][base_index].id,
                .array_index = base_index,
                .movetime = 100
            });

            mq.send_message({
                .white = players[generations[r.generation + 1][base_index + 1].id].hash,
                .black = players[generations[r.generation + 1][base_index + 1].child_id].hash,
                .outputdir = "chess/generations/",
                .generation = r.generation + 1,
                .id = generations[r.generation + 1][base_index].id,
                .array_index = base_index,
                .movetime = 100
            });
        }
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

    p1.child_id = players.size();
    players.push_back(child1);
    p2.child_id = players.size();
    players.push_back(child2);
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

    int winner;
    if (msg.result == "1-0") {
        winner = players[msg.id].hash == msg.white ? msg.id : generations[msg.generation][msg.array_index].child_id;
    } else if (msg.result == "0-1") {
        winner = players[msg.id].hash == msg.black ? msg.id : generations[msg.generation][msg.array_index].child_id;
    } else {
        winner = 0;
    }
    return game_result{
            .generation = msg.generation,
            .array_index = msg.array_index,
            .id = msg.id,
            .winner_id = winner
    };
}

void algorithm::enqueue_game(int generation, int index, bool parent_as_white) {
    if (parent_as_white)
        mq.send_message({
                .bucket = "leonkacowicz",
                .white = players[generations[generation][index].id].hash,
                .black = players[generations[generation][index].child_id].hash,
                .outputdir = "chess/generations/",
                .generation = generation,
                .id = generations[generation][index].id,
                .array_index = index,
                .movetime = 100
        });
    else
        mq.send_message({
                .bucket = "leonkacowicz",
                .white = players[generations[generation][index].child_id].hash,
                .black = players[generations[generation][index].id].hash,
                .outputdir = "chess/generations/",
                .generation = generation,
                .id = generations[generation][index].id,
                .array_index = index,
                .movetime = 100
        });
}
