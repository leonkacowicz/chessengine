#ifndef CHESSENGINE_GAME_H
#define CHESSENGINE_GAME_H


#include <string>
#include <vector>
#include <board.h>

namespace chess {
    namespace core {

        struct game_state {
            board b;
            uint64_t hash;
            int last_irreversible_index;
            move last_move;
        };

        class game {
            std::string initial_pos;
            std::vector<game_state> states;

            game(const board&);
            void do_move(move m);
            void undo_last_move();
            bool is_draw_by_3foldrep();
            bool is_draw_by_50move();
        };
    }
}


#endif //CHESSENGINE_GAME_H
