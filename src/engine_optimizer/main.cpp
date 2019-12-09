#include <iostream>
#include <fstream>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <neuralnet.h>
#include "algorithm.h"

using chess::neural::neuralnet;
using boost::filesystem::path;

const std::vector<int> layers{832, 50, 40, 30, 20, 10, 1};

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> rand_dist;

neuralnet random_net() {
    std::normal_distribution dis(0.0, 1.0);
    std::vector<Eigen::MatrixXd> matrices;
    matrices.reserve(layers.size());

    for (int i = 1; i < layers.size(); i++) {
        int rows = layers[i], cols = layers[i - 1] + 1;
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(rows, cols);
        for (int row = 0; row < rows; row++) for (int col = 0; col < cols; col++) M(row, col) = .1 * dis(mt) / dis(mt);
        matrices.push_back(M);
    }
    return neuralnet(matrices);
}

std::vector<std::string> list_files_with_extension(const std::string& ext, const path& location) {
    std::vector<std::string> result;
    boost::filesystem::directory_iterator enditer;
    for (boost::filesystem::directory_iterator iter(location); iter != enditer; ++iter) {
        if (iter->path().extension() == ext)
            result.push_back(iter->path().filename().string());
    }
    return result;
}

int num_files_with_extension(const std::string& ext, const path& location) {
    int num_files = 0;
    boost::filesystem::directory_iterator enditer;
    for (boost::filesystem::directory_iterator iter(location); iter != enditer; ++iter) {
        if (iter->path().extension() == ext)
            num_files++;
    }
    return num_files;
}

void write_nn_to_file(const neuralnet nn, const path& location) {
    path file = path(location) /= "output.txt";
    nn.output_to_stream(std::ofstream(file.string()));
    boost::process::ipstream out;
    boost::process::system("sha256sum " + file.string(), boost::process::std_out > out);
    std::string new_filename;
    out >> new_filename;
    boost::filesystem::rename(file, path(location) /= new_filename + ".txt");
}

void generate_random_population(int pop_size, const path& location, const path& location2) {
    int num_files = num_files_with_extension(".txt", location) + num_files_with_extension(".txt", location2);

    for (int i = num_files; i < pop_size; i++) {
        auto nn = random_net();
        write_nn_to_file(nn, location);
    }
}

double flip_random_bit(double x) {
    auto* z = (uint64_t*)&x;
    while (true) {
        uint64_t mask = 1uL << uint64_t(rand_dist(mt) % 64);
        *z ^= mask;
        double ret = *((double*) z);
        if (!(std::isnan(ret) || std::isinf(ret) || std::abs(ret) > double(10000.0)))
            return ret;
    }
}

void swap(Eigen::VectorXd& v1, Eigen::VectorXd& v2) {
    int size = v1.size();
    for (int i = 0; i < size; i++) {
        double aux = v1(i);
        v1(i) = v2(i);
        v2(i) = aux;
    }
}

int get_score(const neuralnet& nn1, const neuralnet& nn2) {
    nn1.output_to_stream(std::ofstream("white.txt"));
    nn2.output_to_stream(std::ofstream("black.txt"));

    std::ofstream white_options("white_options.txt");
    white_options << "setoption name evaluator value white.txt";
    white_options.close();

    std::ofstream black_options("black_options.txt");
    black_options << "setoption name evaluator value black.txt";
    black_options.close();

    boost::process::ipstream out;
    boost::process::child c("../arbiter/chessarbiter",
                            "--verbose",
                            "--white-exec", "../engine/chessengine",
                            "--white-input", "white_options.txt",
                            "--white-move-time", "50",
                            "--black-exec", "../engine/chessengine",
                            "--black-input", "black_options.txt",
                            "--black-move-time", "50",
                            boost::process::std_out > out
    );

    std::string line;
    while (out.good() && !out.eof() && std::getline(out, line)) {
        std::cout << line << std::endl;
        if (line == "1-0") {
            c.wait();
            return 1;
        } else if (line == "0-1") {
            c.wait();
            return -1;
        } else if (line == "1/2-1/2") {
            c.wait();
            return 0;
        }
    }
    c.wait();
    return 0;
}

int main() {
    int population = 10;
    int num_generations = 100;

    chess::optimizer::algorithm alg(num_generations, population);
    alg.run();
    std::exit(0);
    path workdir("workdir");
    path first_gen(workdir);
    first_gen /= "gen0";
    if (!boost::filesystem::is_directory(first_gen)) {
        boost::filesystem::create_directories(first_gen);
    }
    auto first_gen_processed_path = path(first_gen) /= "processed";

    if (!boost::filesystem::is_directory(first_gen_processed_path)) {
        boost::filesystem::create_directory(first_gen_processed_path);
    }
    generate_random_population(population, first_gen, first_gen_processed_path);

    for (int gen = 0; gen < num_generations; gen++) {
        auto current_gen_path = (path(workdir) /= "gen" + std::to_string(gen));
        auto next_gen_path = (path(workdir) /= "gen" + std::to_string(gen + 1));

        if (!boost::filesystem::is_directory(next_gen_path)) {
            boost::filesystem::create_directories(next_gen_path);
        }

        auto current_gen_processed_path = path(current_gen_path) /= "processed";

        if (!boost::filesystem::is_directory(current_gen_processed_path)) {
            boost::filesystem::create_directory(current_gen_processed_path);
        }

        auto files_in_generation = list_files_with_extension(".txt", current_gen_path);
        assert(files_in_generation.size() == population);
        std::shuffle(files_in_generation.begin(), files_in_generation.end(), rd);

        for (int i = 0; i < files_in_generation.size(); i += 2) {
            auto parent1 = path(current_gen_path) /= files_in_generation[i];
            auto parent2 = path(current_gen_path) /= files_in_generation[i + 1];
            auto parent1_path = path(current_gen_processed_path) /= parent1.filename();
            auto parent2_path = path(current_gen_processed_path) /= parent2.filename();
            boost::filesystem::rename(parent1, parent1_path);
            boost::filesystem::rename(parent2, parent2_path);

            std::ifstream parent1_ifs(parent1_path.string());
            std::ifstream parent2_ifs(parent2_path.string());
            neuralnet parent1_nn(parent1_ifs);
            neuralnet parent2_nn(parent2_ifs);

            Eigen::VectorXd parent1_vec = parent1_nn.to_eigen_vector();
            Eigen::VectorXd parent2_vec = parent2_nn.to_eigen_vector();

            Eigen::VectorXd child1_vec = parent1_vec;
            Eigen::VectorXd child2_vec = parent2_vec;
            int size = child1_vec.size();
            int mutation_ratio = size / 2;
            for (int i = 0; i < size; i++) {
                if (rand_dist(mt) % 2 == 0) child1_vec(i) = parent2_vec(i);
                if (rand_dist(mt) % 2 == 0) child2_vec(i) = parent1_vec(i);
                if (rand_dist(mt) % mutation_ratio == 0) child1_vec(i) = flip_random_bit(child1_vec(i));
                if (rand_dist(mt) % mutation_ratio == 0) child2_vec(i) = flip_random_bit(child2_vec(i));
            }

            if ((child1_vec - parent1_vec).squaredNorm() + (child2_vec - parent2_vec).squaredNorm() >
                (child1_vec - parent2_vec).squaredNorm() + (child2_vec - parent1_vec).squaredNorm()) {
                swap(child1_vec, child2_vec);
            }

            {
                int score1 = 0;
                neuralnet child1_nn(layers, child1_vec);

                if (rand_dist(mt) % 2 == 0) score1 = get_score(parent1_nn, child1_nn);
                else score1 = -get_score(child1_nn, parent1_nn);

                if (score1 >= 0) {
                    if (score1 > 0) std::cout << "[OPTIMIZER] parent won!" << std::endl;
                    else std::cout << "[OPTIMIZER] draw!" << std::endl;
                    write_nn_to_file(parent1_nn, next_gen_path);
                } else {
                    std::cout << "[OPTIMIZER] child won!" << std::endl;
                    write_nn_to_file(child1_nn, next_gen_path);
                }
            }
            {
                int score2 = 0;
                neuralnet child2_nn(layers, child2_vec);

                if (rand_dist(mt) % 2 == 0) score2 = get_score(parent2_nn, child2_nn);
                else score2 = -get_score(child2_nn, parent2_nn);

                if (score2 >= 0) {
                    if (score2 > 0) std::cout << "[OPTIMIZER] parent won!" << std::endl;
                    else std::cout << "[OPTIMIZER] draw!" << std::endl;
                    write_nn_to_file(parent2_nn, next_gen_path);
                } else {
                    std::cout << "[OPTIMIZER] child won!" << std::endl;
                    write_nn_to_file(child2_nn, next_gen_path);
                }
            }
        }
    }
}