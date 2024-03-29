//
// Created by leon on 2019-11-07.
//

#include <boost/filesystem/path.hpp>
#include <boost/process.hpp>

#include <chess/neuralnet/mlp.h>
#include <iostream>
#include "algorithm.h"
#include "message_queue.h"

using namespace chess::optimizer;
using chess::neural::mlp;

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

    bool finished = false;
    while (!finished) {
        game_result r = get_next_game_result();
        while (r.generation + 1 >= generations.size())
            add_generation();
        int destination = generations[r.generation][r.array_index].destination;
        if (r.winner_id == generations[r.generation][r.array_index].child_id)
            generations[r.generation + 1][destination].id = generations[r.generation][r.array_index].child_id;
        else
            generations[r.generation + 1][destination].id = generations[r.generation][r.array_index].id;

        serialize_state(std::cout);
        int base_index = destination - (destination % 2);
        assert(r.generation + 1 < generations.size());

        if (r.generation < num_generations &&
            generations[r.generation + 1][base_index].id > -1 &&
            generations[r.generation + 1][base_index + 1].id > -1) {
            assert(generations[r.generation + 1][base_index].id < players.size());
            assert(generations[r.generation + 1][base_index + 1].id < players.size());
            cross_over(generations[r.generation + 1][base_index], generations[r.generation + 1][base_index + 1]);
            enqueue_game(r.generation + 1, base_index, true);
            enqueue_game(r.generation + 1, base_index + 1, false);
        }

        if (generations.size() >= num_generations) {
            finished = true;
            for (int i = 0; i < population_size; i++)
                if (generations.back()[i].id == -1) {
                    finished = false;
                    break;
                }
        }
    }
}

algorithm::player algorithm::generate_random_player() {
    return save_player(mlp(rd(), {832, 1}));
}

void algorithm::cross_over(algorithm::parent& p1, algorithm::parent& p2) {
    int theta_size = players[p1.id].theta.size();
    const Eigen::VectorXd& theta1 = players[p1.id].theta;
    const Eigen::VectorXd& theta2 = players[p2.id].theta;
    assert(theta1.size() == theta2.size());
    auto theta1_child = theta1;
    auto theta2_child = theta2;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dis(0, 9);
    std::uniform_int_distribution dis2(0, 99);

    for (int i = 0; i < theta_size; i++) {
        if (dis(mt) == 0) theta1_child(i) = theta2(i);
        else theta1_child(i) = theta1(i);
        if (dis(mt) == 0) theta2_child(i) = theta1(i);
        else theta2_child(i) = theta2(i);

        if (dis2(mt) == 0) {

        }
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
    return save_player(mlp({832, 1}, theta));
}

algorithm::player algorithm::save_player(const mlp& nn) {
    using namespace boost::process;
    algorithm::player p;
    p.theta = nn.to_eigen_vector();
    boost::filesystem::path file = boost::filesystem::unique_path();
    nn.output_to_stream(std::ofstream(file.string()));
    ipstream out;
    system("sha256sum " + file.string(), std_out > out);
    out >> p.hash;
    ipstream out2;
    system(search_path("aws"), "s3", "cp", file.string(), "s3://leonkacowicz/chess/players/" + p.hash + ".txt", std_out > out2);
    boost::filesystem::remove(file);
    return p;
}

algorithm::game_result algorithm::get_next_game_result() {
    using namespace std::chrono_literals;
    response_message msg;
    while (!mq.receive_message(msg, 10s)) std::cout << "Waiting for game results" << std::endl;

    assert(msg.generation < generations.size());
    assert(msg.array_index < population_size);
    assert(generations[msg.generation][msg.array_index].id == msg.id);

    int winner;
    if (msg.result == "1-0") {
        winner = players[msg.id].hash == msg.white ? msg.id : generations[msg.generation][msg.array_index].child_id;
    } else if (msg.result == "0-1") {
        winner = players[msg.id].hash == msg.black ? msg.id : generations[msg.generation][msg.array_index].child_id;
    } else {
        winner = -1;
    }
    return game_result{
            .generation = msg.generation,
            .array_index = msg.array_index,
            .id = msg.id,
            .winner_id = winner
    };
}

void algorithm::enqueue_game(int generation, int index, bool parent_as_white) {
    auto parent_hash = players[generations[generation][index].id].hash;
    auto child_hash = players[generations[generation][index].child_id].hash;

    request_engine_config parent;
    parent.movetime = 50;
    parent.exec = "./chessengine";
    parent.weights_file = parent_hash + ".txt";

    request_engine_config child;
    child.movetime = 50;
    child.exec = "./chessengine";
    child.weights_file = child_hash + ".txt";

    request_message msg;
    msg.bucket = "leonkacowicz";
    msg.outputdir = "chess/2019-12-08-0002/generations/";
    msg.array_index = index;
    msg.generation = generation;
    msg.id = generations[generation][index].id;

    if (parent_as_white) {
        msg.white = parent;
        msg.black = child;
        msg.game_id = parent_hash + "_" + child_hash;
    } else {
        msg.white = child;
        msg.black = parent;
        msg.game_id = child_hash + "_" + parent_hash;
    }
    mq.send_message(msg);
}

void algorithm::serialize_state(std::ostream& os) {
    for (int g = 0; g < generations.size(); g++) {
        os << generations[g][0].id;
        for (int i = 1; i < generations[g].size(); i++) {
            os << "," << generations[g][i].id;
        }
        os << std::endl;
    }
}
