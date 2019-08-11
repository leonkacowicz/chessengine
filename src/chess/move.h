#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H

#include <string>
#include <sstream>
#include "square.h"

using namespace chess::core;
//namespace chess {
//    namespace core {
        enum special_move : char {
            NOT_SPECIAL = 0,
            CASTLE_KING_SIDE_WHITE = 1,
            CASTLE_QUEEN_SIDE_WHITE,
            CASTLE_KING_SIDE_BLACK,
            CASTLE_QUEEN_SIDE_BLACK,
            PROMOTION_QUEEN,
            PROMOTION_ROOK,
            PROMOTION_BISHOP,
            PROMOTION_KNIGHT,
            NULL_MOVE
        };
        typedef uint32_t move;

        constexpr move get_move(square from, square to) {
            return unsigned(from) | (unsigned(to) << 6u);
        }

        constexpr move get_move(square from, square to, special_move special) {
            return get_move(from, to) | (unsigned(special) << 12u);
        }

        constexpr move get_move(const char * from, const char * to) {
            return get_move(get_square(from), get_square(to));
        }

        constexpr move get_move(square from, const char * to) {
            return get_move(from, get_square(to));
        }

        constexpr square move_origin(move m) {
            return square(m & 0b111111u);
        }

        constexpr square move_dest(move m) {
            return square((m >> 6u) & 0b111111u);
        }

        constexpr special_move move_type(move m) {
            return special_move((m >> 12u) & 0b111111u);
        }
        constexpr move null_move = get_move(SQ_NONE, SQ_NONE, NULL_MOVE);

        inline std::ostream& operator<<(std::ostream& os, move m) {
            auto special = move_type(m);
            os << move_origin(m) << move_dest(m);
            if (special == PROMOTION_QUEEN) return os << 'q';
            if (special == PROMOTION_ROOK) return os << 'r';
            if (special == PROMOTION_BISHOP) return os << 'b';
            if (special == PROMOTION_KNIGHT) return os << 'n';
            return os;
        }
        inline std::string to_long_move(move m) {
            auto special = move_type(m);
            if (special == NULL_MOVE) return "(none)";
            std::stringstream ss;
            ss << move_origin(m) << move_dest(m);
            if (special == PROMOTION_QUEEN) ss << 'q';
            if (special == PROMOTION_ROOK) ss << 'r';
            if (special == PROMOTION_BISHOP) ss << 'b';
            if (special == PROMOTION_KNIGHT) ss << 'n';
            return ss.str();
        }

//    }
//}

//class move {
//private:
//
//public:
//    square origin;
//    square destination;
//    char special; // 0, n, r, b, q
//    move() : origin(square()), destination(square()), special(NULL_MOVE) {};
//    constexpr move(square origin, square destination, special_move special = NOT_SPECIAL)
//        : origin(origin), destination(destination), special(special) {};
//
//    constexpr move(const char* origin, const char* dest, special_move special = NOT_SPECIAL) :
//        origin(get_square(origin)), destination(get_square(dest)), special(special) {}
//
//    constexpr move(square origin, const char* dest, special_move special = NOT_SPECIAL) :
//        origin(origin), destination(get_square(dest)), special(special) {}
//
//
//    constexpr bool operator==(const move& rhs) const {
//        return destination == rhs.destination && origin == rhs.origin && special == rhs.special;
//    }
//    std::string to_long_move() const {
//        if (special == NULL_MOVE) return "(none)";
//        std::stringstream ss;
//        ss << origin << destination;
//        if (special == PROMOTION_QUEEN) ss << 'q';
//        if (special == PROMOTION_ROOK) ss << 'r';
//        if (special == PROMOTION_BISHOP) ss << 'b';
//        if (special == PROMOTION_KNIGHT) ss << 'n';
//        return ss.str();
//    }
//};

#endif //CHESSENGINE_MOVE_H
