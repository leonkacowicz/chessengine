#include "game.h"
#include "zobrist.h"

using namespace chess::core;

game::game(const board& b) {
    game_state s;
    s.b = b;
    s.hash = zobrist::hash(b, 0);
    s.last_move = null_move;
    s.last_irreversible_index = 0;
    states.push_back(s);
}

void game::do_move(move m) {
    game_state s;
    s = states.back();
    if (s.b.resets_half_move_counter(m)) {
        s.last_irreversible_index = states.size();
    }
    s.b.make_move(m);
    s.hash = zobrist::hash(s.b, 0);
    s.last_move = m;
    states.push_back(s);
}

void game::undo_last_move() {
    if (states.size() > 1) {
        states.erase(states.end() - 1);
    }
}

bool game::is_draw_by_3foldrep() {
    game_state& last = states.back();
    int reps = 0;
    int size = states.size();
    for (int i = last.last_irreversible_index; i < size; i++) {
        if (states[i].hash == last.hash)
            if (states[i].b == last.b)
                if (++reps >= 3)
                    return true;
    }
    return false;
}

bool game::is_draw_by_50move() {
    return states.back().b.half_move_counter >= 100;
}
