#include "../chess/color.h"
#include <Board.h>
#include <Move.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

bool Board::isCheckmate() const {
    return false;
}

string Board::toString() const {
    auto ret = stringstream();
    for (int y = 7; y >= 0; y--) {
        ret << " " << (y+1) << "  ";
        for (int x = 0; x <= 7; x++) {
            bitboard position(x, y);
            char c = '.';
            if (hasPieceOfColor[WHITE][position] || hasPieceOfColor[BLACK][position]) {
                if (hasPieceOfType[BISHOP][position]) c = 'B';
                else if (hasPieceOfType[ROOK][position]) c = 'R';
                else if (hasPieceOfType[KNIGHT][position]) c = 'N';
                else if (hasPieceOfType[QUEEN][position]) c = 'Q';
                else if (hasPieceOfType[PAWN][position]) c = 'P';
                else c = 'K';
                if (hasPieceOfColor[BLACK][position]) c += 'a' - 'A';
            }
            ret << ' ' << c;
        }
        ret << endl;
    }
    ret << endl << "     a b c d e f g h" << endl;

    return ret.str();
}

void Board::printBoard() const {
    cout << toString();
}

void Board::setInitialPosition()
{
    hasPieceOfType[ROOK] =   0x8100000000000081;
    hasPieceOfType[KNIGHT] = 0x4200000000000042;
    hasPieceOfType[BISHOP] = 0x2400000000000024;
    hasPieceOfType[QUEEN] =  0x1000000000000010;
    hasPieceOfType[PAWN] =   0x00FF00000000FF00;
    hasPieceOfColor[BLACK] = 0xFFFF000000000000;
    hasPieceOfColor[WHITE] = 0x000000000000FFFF;
}

void Board::calculateAttacks() {
    for (auto position = bitboard(1); !position.isEmpty(); position <<= 1)
    {
        if (position[kingPosition[WHITE]] || position[kingPosition[BLACK]])
            calculateKingAttacks(position);

        if (!hasPieceOfColor[WHITE][position] && !hasPieceOfColor[BLACK][position]) continue;

        if (hasPieceOfType[ROOK][position])
            calculateRookAttacks(position);
    }
}

std::vector<Move> Board::getPossibleMovesFor(color color) {
    std::vector<Move> moves;

    for (auto position = bitboard(1); !position.isEmpty(); position <<= 1)
    {
        if (!hasPieceOfColor[color][position]) continue;
        if (hasPieceOfType[ROOK][position]) addRookPossibleMoves(position, moves);
        if (hasPieceOfType[KING][position]) addKingPossibleMoves(position, moves);
    }

    return moves;
}

void Board::calculateKingAttacks(const bitboard origin) {
    color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;

    if (!rank_8[origin]) {
        auto north = origin.shift_up(1);
        if (!hasPieceOfColor[attackerColor][north])
            attacks[attackerColor] |= north;

        if (!file_a[origin]) {
            auto northwest = origin.shift_up_left(1);
            if (!hasPieceOfColor[attackerColor][northwest])
                attacks[attackerColor] |= northwest;
        }

        if (!file_h[origin]) {
            auto northeast = origin.shift_up_right(1);
            if(!hasPieceOfColor[attackerColor][northeast])
                attacks[attackerColor] |= northeast;
        }
    }
    if (!rank_1[origin]) {
        auto south = origin.shift_down(1);
        if (!hasPieceOfColor[attackerColor][south])
            attacks[attackerColor] |= south;

        if (!file_a[origin]) {
            auto southwest = origin.shift_down_left(1);
            if (!hasPieceOfColor[attackerColor][southwest])
                attacks[attackerColor] |= southwest;
        }

        if (!file_h[origin]) {
            auto southeast = origin.shift_down_right(1);
            if (!hasPieceOfColor[attackerColor][southeast])
                attacks[attackerColor] |= southeast;
        }
    }
    if (!file_a[origin]) {
        auto west = origin.shift_left(1);
        if (!hasPieceOfColor[attackerColor][west])
            attacks[attackerColor] |= west;
    }
    if (!file_h[origin]) {
        auto east = origin.shift_right(1);
        if (!hasPieceOfColor[attackerColor][east])
            attacks[attackerColor] |= east;
    }
}

void Board::calculateRookAttacks(const bitboard origin) {
    // naive implementation
    color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = origin;
    while (!rank_8[square]) {
        square <<= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square >>= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square >>= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square <<= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

std::vector<Move> Board::pseudo_legal_rook_moves(const bitboard origin, color attackerColor) const noexcept {
    std::vector<Move> ret;
    color oppositeColor = opposite(attackerColor);
    bitboard square = origin;
    while (!rank_8[square]) {
        square = square.shift_up(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square = square.shift_down(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square = square.shift_left(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square = square.shift_right(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

void Board::putPiece(Piece piece, color color, Square position) {
    if (piece == PAWN) return putPawn(color, position);
    if (piece == BISHOP) return putBishop(color, position);
    if (piece == KNIGHT) return putKight(color, position);
    if (piece == ROOK) return putRook(color, position);
    if (piece == QUEEN) return putQueen(color, position);
    if (piece == KING) return putKing(color, position);
}

void Board::putPawn(color color, Square position) {
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] |= bitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putKight(color color, Square position){
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] |= bitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putBishop(color color, Square position){
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] |= bitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putRook(color color, Square position){
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] |= bitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putQueen(color color, Square position) {
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] |= bitBoard;
}

void Board::putKing(color color, Square position) {
    hasPieceOfColor[color] &= ~(bitboard(kingPosition[color]));
    kingPosition[color] = position;
    auto bitBoard = bitboard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::setPieceColor(color color, bitboard bitBoard) {
    hasPieceOfColor[color] |= bitBoard;
    hasPieceOfColor[opposite(color)] &= ~bitBoard;
}

bitboard Board::getAttacksFrom(color color) const {
    return attacks[color];
}

void Board::addRookPossibleMoves(bitboard origin, std::vector<Move> & moves) {
    color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;
    color oppositeColor = opposite(attackerColor);

    bitboard square = origin;
    while (!rank_8[square]) {
        square <<= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());

        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank_1[square]) {
        square >>= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_a[square]) {
        square <<= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!file_h[square]) {
        square >>= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

void Board::addKingPossibleMoves(bitboard origin, std::vector<Move>& moves) {


}

Board::Board(const std::string& fen) {

    using std::string;
    using std::stringstream;

    stringstream ss(fen);

    string piecePlacement;
    getline(ss, piecePlacement, ' ');

    string side_to_move;
    getline(ss, side_to_move, ' ');

    string castling;
    getline(ss, castling, ' ');

    string en_passant;
    getline(ss, en_passant, ' ');

    string half_move_clock;
    getline(ss, half_move_clock, ' ');

    string full_move_counter;
    getline(ss, full_move_counter, ' ');

    sideToPlay = side_to_move == "b";
}

