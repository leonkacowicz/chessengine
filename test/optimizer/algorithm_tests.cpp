//
// Created by leon on 2019-11-24.
//

#include <gtest/gtest.h>
#include <algorithm.h>
#include <mlp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <particle_swarm_optimizer.h>
#include <supervised_trainer.h>
#include "../test_common.h"

using namespace chess::optimizer;
using namespace chess::neural;

TEST(ptree_test, ptree_test1) {
    using namespace boost::property_tree;

    ptree json;
    std::stringstream ss;
    ss << R"({
    "Messages": [
        {
            "MessageId": "13fa6165-a669-4d91-8a4d-f69c63a98b4b",
            "ReceiptHandle": "AQEBquqQzliyjPRkdAxi/bIvxqKUjrsTcPbxuJDWDGyPxHmLs4hmP+mhxuPaqJ7lZyY2A3Htv5f+nZ2Y1PWZRdAZTHgeiZ0yFhRa1bCID1s/Fa7MHcMgrKp3/vVo+No+kd7P9eFoZSh1nx8S/B6+3i2k74fdAm0cRCn8daW3FJNrPDdd4xd3aXKquIDdIKtVJRJ5tXk6/atXEYfkmddtXgUWKdIKxNJhlQYwZEWp4PcZOtzvz2fJbv0XffcIj/SBhT2NpCTml2oIt+6BiM7g3SIPufCT+de3hMJLnzt7Bb+TNYKBze9Jh8Six5RploBi3I2/D/qYtjk8tAEVCTqYWy8tdo6dxsTwvClgpEl+RqPeIPyM7lkcrVziim0qcSuiTm9amNj/QrUpCL1zov/sADM3tg==",
            "MD5OfBody": "096b20222eda1e0fa35c9ef6a157795e",
            "Body": "{\n  \"bucket\": \"leonkacowicz\",\n  \"outputdir\": \"chess/generations/\",\n  \"white\": \"3f0c559dfa7954d375835967edb155587c0091dcb62c2b5de44b194a25934268\",\n  \"black\": \"bccc97094406a7dd19fa5f785301f1fd34bf2b7d9aafd2a91942981e74bc7d48\",\n  \"movetime\": \"100\",\n  \"result\": \"1-0\"\n}"
        }
    ]
}
)";
    read_json(ss, json);
    //std::cout << json.get_child("Messages.0.ReceiptHandle").data();
    for (auto& msg : json.get_child("Messages")) {
        std::cout << msg.second.get<std::string>("ReceiptHandle").data() << std::endl;
        ptree body;

        std::stringstream ss2(msg.second.get<std::string>("Body"));
        read_json(ss2, body);
        for (auto& field : body) {
            std::cout << field.first << " = " << field.second.data() << std::endl;
        }
    }

    std::cout << json.size();
}

double eval_xor(const mlp& nn) {
    double e = 0;
    auto squared = [] (auto x) { return x * x; };
    e += squared(nn({0, 0})[0]);
    e += squared(nn({1, 1})[0]);
    e += squared(1 - nn({0, 1})[0]);
    e += squared(1 - nn({1, 0})[0]);
    return std::sqrt(e / 4);
}

double eval_xor3(const mlp& nn) {
    double e = 0;
    auto squared = [] (auto x) { return x * x; };
    e += squared(-1.0 - nn(Eigen::Vector3d(0, 0, 0))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(0, 0, 1))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(0, 1, 0))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(0, 1, 1))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(1, 0, 0))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(1, 0, 1))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(1, 1, 0))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(1, 1, 1))[0]);
    return std::sqrt(e);
}

TEST(algorithm_test, particle_swarm) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif(0., 1.);
    const int T = 1500;
    int P = 50;
    auto layers = {3, 5, 1};
    int D = mlp::num_vector_dimensions(layers);
    std::vector<Eigen::VectorXd> positions(P, Eigen::VectorXd::Random(D));
    std::vector<Eigen::VectorXd> best_pos = positions;
    std::vector<double> best_values;
    std::vector<Eigen::VectorXd> velocities(P, Eigen::VectorXd::Random(D) / 1000);

    int cmp = 0;
    int best = 0;
    double best_value = INFINITY;
    for (int i = 0; i < P; i++) {
        const auto& p = positions[i];
        auto val = eval_xor3(mlp(layers, p));
        best_values.push_back(val);
        cmp++;
//        if (val < best_value) {
//            best_value = val;
//            best = i;
//        }
    }

    for (int t = 0; t < T; t++) {
        std::printf("%d [%d]: ", t, cmp);
        for (int p = 0; p < P; p++) {
            for (int d = 0; d < D; d++) {
                double rp = unif(mt);
                double rg = unif(mt);
                double rr = unif(mt);
                velocities[p][d] = 0.95 * velocities[p][d] +
                        0.2 * rp * (best_pos[p][d] - positions[p][d]) +
                        0.2 * rg * (best_pos[best][d] - positions[p][d]) +
                        0.1 * (rr - .5);
            }
            positions[p] += velocities[p];
            auto val = eval_xor3(mlp(layers, positions[p]));
            cmp++;
            if (val < best_values[p]) {
                best_values[p] = val;
                best_pos[p] = positions[p];
                cmp++;
                if (val < best_value) {
                    best = p;
                    best_value = val;
                    //std::printf("*");
                }
            }
            std::printf("%.4f, ", best_values[p]);
        }
        std::printf("%.4f\n", best_value);
    }
}



TEST(algorithm_test, particle_swarm2) {
    particle_swarm_optimizer<double> pso;
    pso.vector_size = mlp::num_vector_dimensions({3, 3, 1});
    pso.steps = 1;
    pso.inertia = .95;
    pso.local_best_param = .2;
    pso.global_best_param = .2;
    pso.random_vec_param = 0.1;
    std::atomic<int> cmps = 0;
    for (int i = 0; i < 3000; i++) {
        pso.run([] (const Eigen::VectorXd& x) { return eval_xor3(mlp({3, 3, 1}, x)); }, [&] (double x1, double x2) { ++cmps; return x2 < x1; });
        std::cout << cmps;
        for (int p = 0; p < pso.particle_count; p++)
            std::printf(", %.4f", pso.particles_best_values[p]);
        std::cout << std::endl;
    }
}

TEST(supervised_trainer, train_xor) {

    supervised_trainer st({3, 3, 1});
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << -1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << 1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << 1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << -1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << 1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << -1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << -1.0).finished());
    st.training_set_outputs.push_back((Eigen::VectorXd(1) << 1.0).finished());

    st.training_set.push_back((Eigen::VectorXd(3) << 0, 0, 0).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 0, 0, 1).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 0, 1, 0).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 0, 1, 1).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 1, 0, 0).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 1, 0, 1).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 1, 1, 0).finished());
    st.training_set.push_back((Eigen::VectorXd(3) << 1, 1, 1).finished());
    /*
    e += squared(-1.0 - nn(Eigen::Vector3d(0, 0, 0))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(0, 0, 1))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(0, 1, 0))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(0, 1, 1))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(1, 0, 0))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(1, 0, 1))[0]);
    e += squared(-1.0 - nn(Eigen::Vector3d(1, 1, 0))[0]);
    e += squared(1.0 - nn(Eigen::Vector3d(1, 1, 1))[0]);
     */

    st.train(300);
}

Eigen::VectorXd crossover(const Eigen::VectorXd& v1, const Eigen::VectorXd& v2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dist(0, 1);
    auto size = v1.size();
    Eigen::VectorXd r = v1;
    for (int i = 0; i < size; i++) {
        double p = dist(mt) * dist(mt);
        r[i] = (1 - p) * v1[i] + p * v2[i];
    }
    return r;
}

Eigen::VectorXd mutate(const Eigen::VectorXd& v1) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution dist(.0, 1.0);
    auto size = v1.size();
    Eigen::VectorXd r = v1;
    for (int i = 0; i < size; i++) {
        double p = dist(mt);
        if (p < 0.1)
            r[i] += (dist(mt) - .5);
    }
    return r;
}


TEST(algorithm_test, detcrowd) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution unif;
    const int inputs = 3;
    const std::vector<int> layers{3, 3, 1};
    const int vec_size = mlp::num_vector_dimensions(layers);
    int pop_size = 100;

    std::vector<std::pair<double, Eigen::VectorXd>> population;
    for (int i = 0; i < pop_size; i++) {
        auto theta = Eigen::VectorXd::Random(vec_size);
        population.emplace_back(eval_xor3(mlp(layers, theta)), theta);
    }

    int improved_after_cross = 0;
    int improved_after_no_cross = 0;
    int total_evals = 0;
    for (int generation = 1; generation < 1500; generation++) {
        auto permutation = randomperm(pop_size);
        std::vector<std::pair<double, Eigen::VectorXd>> new_population(pop_size, std::make_pair(0, Eigen::VectorXd::Random(vec_size)));
        for (int i = 0; i < pop_size - 1; i += 2) {
            for (int j = 0; j < 2; j++) {
                Eigen::VectorXd child = crossover(population[i + j].second, population[i + 1 - j].second);
                double eval = eval_xor3(mlp(layers, child));
                total_evals++;
                if (eval < population[i + j].first) {
                    new_population[permutation[i + j]] = std::make_pair(eval, child);
                    improved_after_cross++;
                } else {
                    child = population[i + j].second;
                    child = mutate(child);
                    eval = eval_xor3(mlp(layers, child));
                    total_evals++;
                    if (eval < population[i + j].first) {
                        new_population[permutation[i + j]] = std::make_pair(eval, child);
                        improved_after_no_cross++;
                    } else
                        new_population[permutation[i + j]] = std::make_pair(population[i + j].first, population[i + j].second);
                }
            }
        }
        population = new_population;
        double min = 10000;
        int min_idx = -1;
        double avg = 0;
        for (int i = 0; i < pop_size; i++) {
            if (population[i].first < min) {
                min = population[i].first;
                min_idx = i;
            }
            avg += population[i].first;
        }
        avg /= pop_size;
        std::cout << generation << "[" << total_evals << "]: ";
        std::cout << min << "; " << avg;
        std::cout << std::endl;
    }
    std::cout << "improved after crossover: " << improved_after_cross
              << "; improved after no crossover: " << improved_after_no_cross
              << "; total evaluations: " << total_evals
              << std::endl
              << std::endl;
    std::cout << population[0].second;
}