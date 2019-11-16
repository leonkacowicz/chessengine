//
// Created by leon on 2019-07-30.
//

#include "zobrist.h"

uint64_t zobrist::table[64][6][2];
uint64_t zobrist::side = 0;
uint64_t zobrist::castling_rights[4];
uint64_t zobrist::en_passant[16];
uint64_t zobrist::depth_hash[100];
