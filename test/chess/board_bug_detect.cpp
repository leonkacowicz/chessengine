#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by leon on 2019-07-31.
//

#include <gtest/gtest.h>
#include <move.h>
#include <board.h>
#include <fen.h>

TEST(board_test, bug_detector) {
    std::default_random_engine gen;
    std::uniform_int_distribution<int> dis(1, 20000);
    dis(gen);
    int testNum;
    for (testNum = 0; testNum < 50000; testNum++) {
        board b;
        b.set_initial_position();
        std::vector<move> legal_moves = b.get_legal_moves(b.side_to_play);
        int k = 0;
        if (testNum % 1000 == 0) std::cout << "Tested " << testNum << " cases." << std::endl;

        std::vector<std::string> pgn;
        while (!legal_moves.empty() && k < 200) {
            k++;
            try {
                unsigned long i = dis(gen) % legal_moves.size();
                move m = legal_moves[i];
                pgn.push_back(b.move_in_pgn(m, legal_moves));
                b.make_move(m);
                if (b.under_check(opposite(b.side_to_play))) {
                    std::__throw_runtime_error("Move ignored check");
                }
                if (!(b == board(fen::to_string(b)))) std::__throw_runtime_error("FEN failed");

                legal_moves = b.get_legal_moves(b.side_to_play);
            } catch (std::exception& e) {
                std::cout << "Exception: " << e.what() << std::endl;
                std::cout << std::endl;
                for (int j = 0; j < pgn.size(); j++)
                    (j % 2 == 0 ? std::cout << j / 2 + 1 << ". " : std::cout) << pgn[j] << " ";
                std::cout << std::endl;
                std::cout << "======================================================";
                std::cout << std::endl;
                std::cout << std::endl;
                break;
            }
        }
    }

    std::cout << "Tested " << testNum << " cases.\n";
}
#pragma clang diagnostic pop