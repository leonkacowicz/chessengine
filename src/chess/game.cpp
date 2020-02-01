#include <chess/game.h>
#include <chess/zobrist.h>

using namespace chess::core;

game::game() {
    game_state s;
    s.b.set_initial_position();
    s.hash = zobrist::hash(s.b);
    s.last_move = null_move;
    s.last_irreversible_index = 0;
    states.push_back(s);
}

game::game(board b) {
    game_state s{.b = b, .hash = zobrist::hash(b), .last_irreversible_index = 0, .last_move = null_move};
    states.push_back(s);
}

void game::do_move(move m) {
    game_state s;
    s = states.back();
    if (s.b.resets_half_move_counter(m)) {
        s.last_irreversible_index = states.size();
    }
    s.hash = zobrist::hash_update(s.b, s.hash, m);
    s.b.make_move(m);
    s.last_move = m;
    states.push_back(s);
}

void game::do_null_move() {
    game_state s;
    s = states.back();
    s.b.side_to_play = opposite(s.b.side_to_play);
    s.hash ^= zobrist::side;
    s.last_move = null_move;
    states.push_back(s);
}

void game::undo_last_move() {
    if (states.size() > 1) {
        states.erase(states.end() - 1);
    }
}

bool game::is_draw_by_3foldrep() {
    game_state& last = states.back();
    int reps = 1;
    int size = states.size();
    for (int i = size - 3; i >= last.last_irreversible_index; i -= 2) {
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

bool game::is_draw_by_insufficient_material() {
    auto b = states.back().b;
    if (b.piece_of_type[QUEEN] || b.piece_of_type[ROOK] || b.piece_of_type[PAWN]) return false;
    int light_square_bishops_white = num_squares(b.piece_of_type[BISHOP] & b.piece_of_color[WHITE] & light_squares);
    int light_square_bishops_black = num_squares(b.piece_of_type[BISHOP] & b.piece_of_color[BLACK] & light_squares);
    int dark_square_bishops_white = num_squares(b.piece_of_type[BISHOP] & b.piece_of_color[WHITE] & dark_squares);
    int dark_square_bishops_black = num_squares(b.piece_of_type[BISHOP] & b.piece_of_color[BLACK] & dark_squares);
    int knights_white = num_squares(b.piece_of_type[KNIGHT] & b.piece_of_color[WHITE]);
    int knights_black = num_squares(b.piece_of_type[KNIGHT] & b.piece_of_color[BLACK]);

    if (light_square_bishops_white + dark_square_bishops_white + light_square_bishops_black + dark_square_bishops_black == 0) {
        if (knights_white == 0 && knights_black <= 2) return true;
        else if (knights_black == 0 && knights_white <= 2) return true;
    } else if (knights_white + knights_black == 0) {
        if (light_square_bishops_white + light_square_bishops_black == 0) return true;
        else if (dark_square_bishops_white + dark_square_bishops_black == 0) return true;
        else if (light_square_bishops_white + dark_square_bishops_white <= 1 &&
                 light_square_bishops_black + dark_square_bishops_black <= 1) return true;
    }
    return false;
}

bool game::is_draw_by_stale_mate() {
    return false;
}

bool game::is_draw() {
    return is_draw_by_50move() || is_draw_by_insufficient_material() || is_draw_by_stale_mate() || is_draw_by_3foldrep();
}
