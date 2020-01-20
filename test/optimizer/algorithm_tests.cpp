//
// Created by leon on 2019-11-24.
//

#include <gtest/gtest.h>
#include <algorithm.h>
#include <mlp.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace chess::optimizer;
using namespace chess::neural;

TEST(algorithm_test, add_generation_test) {

    algorithm a(3, 4);
    a.run();
}

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
    const int T = 5000;
    int P = 30;
    auto layers = {3, 3, 1};
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
        if (val < best_value) {
            best_value = val;
            best = i;
        }
    }


    for (int t = 0; t < T; t++) {
        std::printf("%d [%d]: ", t, cmp);
        for (int p = 0; p < P; p++) {
            for (int d = 0; d < D; d++) {
                double rp = unif(mt);
                double rg = unif(mt);
                double rr = unif(mt);
                velocities[p][d] = 0.99 * velocities[p][d] +
                        0.05 * rp * (best_pos[p][d] - positions[p][d]) +
                        0.05 * rg * (best_pos[best][d] - positions[p][d]);
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
                    std::printf("*");
                }
            }
            std::printf("%.4f, ", best_values[p]);
        }
        std::printf("%.4f\n", best_value);
    }
}