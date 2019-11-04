//
// Created by leon on 2019-08-18.
//

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Eigen>
#include <iostream>
#include <fstream>

int main() {
    auto M = Eigen::MatrixXf::Random(2, 3);



    /*

        while (true) {
            forma n pares de jogadores sem reposição
            para cada par, roda o jogo

            se o jogo terminar em vitoria, colocar o vencedor no conjunto V e o perdedor no conjunto P
            se o jogo terminar em empate, colocar ambos no conjunto E

            formar N pares de jogadores sorteando
                - do conjunto V com probabilidade 0.70,
                - do conjunto E com probabilidade 0.25
                - do conjunto P com probabilidade 0.05

            para cada par, fazer o cruzamento dos genes, e salvar os novos individuos

            se |P| >= N, deletar N jogadores aleatórios em P do disco
            se não, deletar todos os jogadores de P, e N - |P| jogadores aleatórios de E
        }
     */
//    std::ifstream file("test.txt");
//
//    for (int i = 0; i < 2; i++)
//        for (int j = 0; j < 3; j++)
//            file >> M(i, j);
//    file.close();

    Eigen::VectorXf v {{ 1, 2, 3 }} ;

    Eigen::VectorXf w(3);

    w << M * v, 1;

    std::cout << M << std::endl;
    std::cout << v << std::endl;
    std::cout << M * v << std::endl;
    std::cout << w << std::endl;
    Eigen::ArrayXf ones(3);
    ones << 1, 1, 1;

    std::cout << (w.array() < 1).matrix().cast<double>() * 5.1 << std::endl;

}