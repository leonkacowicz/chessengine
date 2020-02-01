//
// Created by leon on 2019-07-30.
//

#include <chess/zobrist.h>
#include <random>

using namespace chess::core;

uint64_t zobrist::table[64][6][2];
uint64_t zobrist::side = 0;
uint64_t zobrist::castling_rights[4];
uint64_t zobrist::en_passant[16];

void zobrist::init() {
    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> dis(0, (uint64_t)-1);
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 6; j++) {
            table[i][j][0] = dis(gen);
            table[i][j][1] = dis(gen);
        }
    }
    side = dis(gen);
    for (int i = 0; i < 4; i++) castling_rights[i] = dis(gen);
    for (int i = 0; i < 16; i++) en_passant[i] = dis(gen);
}

uint64_t zobrist::hash(const board& b) {
    uint64_t h = 0;
    h ^= b.can_castle_king_side[WHITE] ? castling_rights[0] : 0;
    h ^= b.can_castle_king_side[BLACK] ? castling_rights[1] : 0;
    h ^= b.can_castle_queen_side[WHITE] ? castling_rights[2] : 0;
    h ^= b.can_castle_queen_side[BLACK] ? castling_rights[3] : 0;

    if (b.en_passant >= SQ_A3 && b.en_passant <= SQ_H3) h ^= en_passant[b.en_passant - SQ_A3];
    else if (b.en_passant >= SQ_A6 && b.en_passant <= SQ_H6) h ^= en_passant[b.en_passant - SQ_A6 + 8];

    bitboard sq(1);
    for (int i = 0; i < 64; i++, sq <<= 1u) {
        piece p = b.piece_at(sq);
        if (p == NO_PIECE) continue;
        color c = b.color_at(sq);
        h ^= table[i][p][c];
    }
    if (b.side_to_play == BLACK) return h ^ side;
    return h;
}

uint64_t zobrist::hash_update(const board& b, uint64_t hash, move m) {
    square sq_org = move_origin(m);
    square sq_dst = move_dest(m);
    bitboard bb_org = get_bb(sq_org);
    bitboard bb_dst = get_bb(sq_dst);
    piece p = b.piece_at(bb_org);
    color c = b.color_at(bb_org);
    square new_en_passant = SQ_NONE;

    auto type = move_type(m);
    if (type == special_move::NOT_SPECIAL) {
        hash ^= table[sq_org][p][c] ^ table[sq_dst][p][c];
        piece capture = b.piece_at(bb_dst);
        if (capture < NO_PIECE) hash ^= table[sq_dst][capture][opposite(c)];
        if (p == PAWN) {
            if (sq_dst == b.en_passant)
                hash ^= table[get_square(get_file(sq_dst), get_rank(sq_org))][PAWN][opposite(c)];
            else if (get_rank(sq_dst) == get_rank(sq_org) + 2) new_en_passant = get_square(get_file(move_origin(m)), get_rank(move_origin(m)) + 1);
            else if (get_rank(sq_dst) == get_rank(sq_org) - 2) new_en_passant = get_square(get_file(move_origin(m)), get_rank(move_origin(m)) - 1);
        }
    } else if (type == special_move::CASTLE_KING_SIDE_WHITE) {
        hash ^= table[sq_org][KING][WHITE] ^ table[SQ_G1][KING][WHITE] ^ table[SQ_H1][ROOK][WHITE] ^ table[SQ_F1][ROOK][WHITE];
    } else if (type == special_move::CASTLE_KING_SIDE_BLACK) {
        hash ^= table[sq_org][KING][BLACK] ^ table[SQ_G8][KING][BLACK] ^ table[SQ_H8][ROOK][BLACK] ^ table[SQ_F8][ROOK][BLACK];
    } else if (type == special_move::CASTLE_QUEEN_SIDE_WHITE) {
        hash ^= table[sq_org][KING][WHITE] ^ table[SQ_C1][KING][WHITE] ^ table[SQ_A1][ROOK][WHITE] ^ table[SQ_D1][ROOK][WHITE];
    } else if (type == special_move::CASTLE_QUEEN_SIDE_BLACK) {
        hash ^= table[sq_org][KING][BLACK] ^ table[SQ_C8][KING][BLACK] ^ table[SQ_A8][ROOK][BLACK] ^ table[SQ_D8][ROOK][BLACK];
    } else if (type == special_move::PROMOTION_QUEEN) {
        hash ^= table[sq_org][p][c] ^ table[sq_dst][QUEEN][c];
        piece capture = b.piece_at(bb_dst);
        if (capture < NO_PIECE) hash ^= table[sq_dst][capture][opposite(c)];
    } else if (type == special_move::PROMOTION_BISHOP) {
        hash ^= table[sq_org][p][c] ^ table[sq_dst][BISHOP][c];
        piece capture = b.piece_at(bb_dst);
        if (capture < NO_PIECE) hash ^= table[sq_dst][capture][opposite(c)];
    } else if (type == special_move::PROMOTION_ROOK) {
        hash ^= table[sq_org][p][c] ^ table[sq_dst][ROOK][c];
        piece capture = b.piece_at(bb_dst);
        if (capture < NO_PIECE) hash ^= table[sq_dst][capture][opposite(c)];
    } else if (type == special_move::PROMOTION_KNIGHT) {
        hash ^= table[sq_org][p][c] ^ table[sq_dst][KNIGHT][c];
        piece capture = b.piece_at(bb_dst);
        if (capture < NO_PIECE) hash ^= table[sq_dst][capture][opposite(c)];
    }

    if (b.can_castle_king_side[WHITE] && (sq_org == SQ_H1 || sq_dst == SQ_H1 || sq_org == SQ_E1)) hash ^= castling_rights[0];
    if (b.can_castle_king_side[BLACK] && (sq_org == SQ_H8 || sq_dst == SQ_H8 || sq_org == SQ_E8)) hash ^= castling_rights[1];
    if (b.can_castle_queen_side[WHITE] && (sq_org == SQ_A1 || sq_dst == SQ_A1 || sq_org == SQ_E1)) hash ^= castling_rights[2];
    if (b.can_castle_queen_side[BLACK] && (sq_org == SQ_A8 || sq_dst == SQ_A8 || sq_org == SQ_E8)) hash ^= castling_rights[3];

    if (b.en_passant >= SQ_A3 && b.en_passant <= SQ_H3) hash ^= en_passant[b.en_passant - SQ_A3];
    else if (b.en_passant >= SQ_A6 && b.en_passant <= SQ_H6) hash ^= en_passant[b.en_passant - SQ_A6 + 8];
    if (new_en_passant >= SQ_A3 && new_en_passant <= SQ_H3) hash ^= en_passant[new_en_passant - SQ_A3];
    else if (new_en_passant >= SQ_A6 && new_en_passant <= SQ_H6) hash ^= en_passant[new_en_passant - SQ_A6 + 8];

    hash ^= side;
    
    return hash;
}
