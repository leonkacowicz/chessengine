#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <Eigen/Core>
#include <neuralnet.h>

using chess::neural::neuralnet;
using boost::filesystem::path;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> rand_dist;

neuralnet random_net() {
    neuralnet nn({Eigen::MatrixXd::Random(50, 833), Eigen::MatrixXd::Random(1, 51) / 10});
    return nn;
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
        if (!(std::isnan(ret) || std::isinf(ret) || std::abs(ret) > 10 * std::abs(x)))
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

int get_score(const neuralnet nn1, const neuralnet nn2) {
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
                           "--white-move-time", "100",
                           "--black-exec", "../engine/chessengine",
                           "--black-input", "black_options.txt",
                           "--black-move-time", "100",
                           boost::process::std_out > out
    );

    std::string line;
    while (c.running() && out.good() && std::getline(out, line)) {
        std::cout << line << std::endl;
        if (line == "1-0") return 1;
        else if (line == "0-1") return -1;
        else if (line == "1/2-1/2") return 0;
    }
    c.wait();
}

int main() {
    int population = 4;
    int num_generations = 15;

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

        while (true) {
            int num_files = num_files_with_extension(".txt", current_gen_path);
            if (num_files >= 2) {
                boost::filesystem::directory_iterator end_iter;
                boost::filesystem::directory_iterator iter(current_gen_path);
                while (iter->path().extension() != ".txt") ++iter;
                auto parent1 = *iter++;
                while (iter->path().extension() != ".txt") ++iter;
                auto parent2 = *iter;

                boost::filesystem::rename(parent1.path(), path(current_gen_processed_path) /= parent1.path().filename());
                boost::filesystem::rename(parent2.path(), path(current_gen_processed_path) /= parent2.path().filename());

                path parent1_path(current_gen_processed_path); parent1_path /= parent1.path().filename();
                path parent2_path(current_gen_processed_path); parent2_path /= parent2.path().filename();

                std::ifstream parent1_ifs(parent1_path.string());
                std::ifstream parent2_ifs(parent2_path.string());
                neuralnet parent1_nn(parent1_ifs);
                neuralnet parent2_nn(parent2_ifs);

                Eigen::VectorXd parent1_vec = parent1_nn.to_eigen_vector();
                Eigen::VectorXd parent2_vec = parent2_nn.to_eigen_vector();

                Eigen::VectorXd child1_vec = parent1_vec;
                Eigen::VectorXd child2_vec = parent2_vec;
                int size = child1_vec.size();
                int mid = size / 2;
                for (int i = mid; i < size; i++) {
                    child1_vec(i) = parent2_vec(i);
                    child2_vec(i) = parent1_vec(i);
                    if (rand_dist(mt) % 1000 == 0) {
                        child1_vec(i) = flip_random_bit(child1_vec(i));
                    }
                    if (rand_dist(mt) % 1000 == 0) {
                        child2_vec(i) = flip_random_bit(child2_vec(i));
                    }
                }

                if ((child1_vec - parent1_vec).squaredNorm() + (child2_vec - parent2_vec).squaredNorm() >
                        (child1_vec - parent2_vec).squaredNorm() + (child2_vec - parent1_vec).squaredNorm()) {
                    swap(child1_vec, child2_vec);
                }

                {
                    int score1 = 0;
                    neuralnet child1_nn({832, 50, 1}, child1_vec);

                    if (rand_dist(mt) % 2 == 0) score1 = get_score(parent1_nn, child1_nn);
                    else score1 = -get_score(child1_nn, parent1_nn);

                    if (score1 >= 0) {
                        write_nn_to_file(parent1_nn, next_gen_path);
                    } else {
                        write_nn_to_file(child1_nn, next_gen_path);
                    }
                }
                {
                    int score2 = 0;
                    neuralnet child2_nn({832, 50, 1}, child2_vec);

                    if (rand_dist(mt) % 2 == 0) score2 = get_score(parent2_nn, child2_nn);
                    else score2 = -get_score(child2_nn, parent2_nn);

                    if (score2 >= 0) {
                        write_nn_to_file(parent2_nn, next_gen_path);
                    } else {
                        write_nn_to_file(child2_nn, next_gen_path);
                    }
                }
            } else {
                break;
            }
        }
    }

    std::exit(0);
    random_net().output_to_stream(std::ofstream("test_white.txt"));
    random_net().output_to_stream(std::ofstream("test_black.txt"));

    std::ofstream white_options("white_options.txt");
    white_options << "setoption name evaluator value test_white.txt";
    white_options.close();

    std::ofstream black_options("black_options.txt");
    black_options << "setoption name evaluator value test_black.txt";
    black_options.close();

//    generate_random_population(0, "~/workdir");

    boost::process::system("../arbiter/chessarbiter",
            "--verbose",
            "--white-exec", "../engine/chessengine",
            "--white-input", "white_options.txt",
            "--white-move-time", "500",
            "--black-exec", "../engine/chessengine",
            "--black-input", "black_options.txt",
            "--black-move-time", "500"
    );

}