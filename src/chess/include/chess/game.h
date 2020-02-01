#ifndef CHESSENGINE_GAME_H
#define CHESSENGINE_GAME_H


#include <string>
#include <vector>
#include <chess/board.h>

namespace chess::core {

    struct game_state {
        board b;
        uint64_t hash;
        int last_irreversible_index;
        move last_move;
    };

    class game {
    public:
        std::string initial_pos;
        std::vector<game_state> states;

        game();
        game(board);

        void do_move(move m);

        void do_null_move();

        void undo_last_move();

        bool is_draw_by_3foldrep();

        bool is_draw_by_50move();

        bool is_draw_by_insufficient_material();

        bool is_draw_by_stale_mate();

        bool is_draw();
    };

    struct auto_undo_last_move {
        game& g;

        explicit auto_undo_last_move(game& g) : g(g) {}

        ~auto_undo_last_move() {
            g.undo_last_move();
        }
    };
}


#endif //CHESSENGINE_GAME_H
