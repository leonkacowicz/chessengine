#include <Color.h>
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
            BitBoard position(x, y);
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
    for (auto position = BitBoard(1); !position.isEmpty(); position <<= 1)
    {
        if (position[kingPosition[WHITE]] || position[kingPosition[BLACK]])
            calculateKingAttacks(position);

        if (!hasPieceOfColor[WHITE][position] && !hasPieceOfColor[BLACK][position]) continue;

        if (hasPieceOfType[ROOK][position])
            calculateRookAttacks(position);
    }
}

std::vector<Move> Board::getPossibleMovesFor(Color color) {
    std::vector<Move> moves;

    for (auto position = BitBoard(1); !position.isEmpty(); position <<= 1)
    {
        if (!hasPieceOfColor[color][position]) continue;
        if (hasPieceOfType[ROOK][position]) addRookPossibleMoves(position, moves);
        if (hasPieceOfType[KING][position]) addKingPossibleMoves(position, moves);
    }

    return moves;
}

void Board::calculateKingAttacks(const BitBoard origin) {
    Color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;

    if (!rank8[origin]) {
        auto north = origin.shiftUp(1);
        if (!hasPieceOfColor[attackerColor][north])
            attacks[attackerColor] |= north;

        if (!fileA[origin]) {
            auto northwest = origin.shiftUpLeft(1);
            if (!hasPieceOfColor[attackerColor][northwest])
                attacks[attackerColor] |= northwest;
        }

        if (!fileH[origin]) {
            auto northeast = origin.shiftUpRight(1);
            if(!hasPieceOfColor[attackerColor][northeast])
                attacks[attackerColor] |= northeast;
        }
    }
    if (!rank1[origin]) {
        auto south = origin.shiftDown(1);
        if (!hasPieceOfColor[attackerColor][south])
            attacks[attackerColor] |= south;

        if (!fileA[origin]) {
            auto southwest = origin.shiftDownLeft(1);
            if (!hasPieceOfColor[attackerColor][southwest])
                attacks[attackerColor] |= southwest;
        }

        if (!fileH[origin]) {
            auto southeast = origin.shiftDownRight(1);
            if (!hasPieceOfColor[attackerColor][southeast])
                attacks[attackerColor] |= southeast;
        }
    }
    if (!fileA[origin]) {
        auto west = origin.shiftLeft(1);
        if (!hasPieceOfColor[attackerColor][west])
            attacks[attackerColor] |= west;
    }
    if (!fileH[origin]) {
        auto east = origin.shiftRight(1);
        if (!hasPieceOfColor[attackerColor][east])
            attacks[attackerColor] |= east;
    }
}

void Board::calculateRookAttacks(const BitBoard origin) {
    // naive implementation
    Color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;
    Color oppositeColor = opposite(attackerColor);

    BitBoard square = origin;
    while (!rank8[square]) {
        square <<= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank1[square]) {
        square >>= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileA[square]) {
        square >>= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileH[square]) {
        square <<= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        attacks[attackerColor] |= square;
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

std::vector<Move> Board::pseudo_legal_rook_moves(const BitBoard origin, Color attackerColor) const noexcept {
    std::vector<Move> ret;
    Color oppositeColor = opposite(attackerColor);
    BitBoard square = origin;
    while (!rank8[square]) {
        square = square.shiftUp(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank1[square]) {
        square = square.shiftDown(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileA[square]) {
        square = square.shiftLeft(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileH[square]) {
        square = square.shiftRight(1);
        if (hasPieceOfColor[attackerColor][square]) break;
        ret.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

void Board::putPiece(Piece piece, Color color, Square position) {
    if (piece == PAWN) return putPawn(color, position);
    if (piece == BISHOP) return putBishop(color, position);
    if (piece == KNIGHT) return putKight(color, position);
    if (piece == ROOK) return putRook(color, position);
    if (piece == QUEEN) return putQueen(color, position);
    if (piece == KING) return putKing(color, position);
}

void Board::putPawn(Color color, Square position) {
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] |= bitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putKight(Color color, Square position){
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] |= bitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putBishop(Color color, Square position){
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] |= bitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putRook(Color color, Square position){
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] |= bitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::putQueen(Color color, Square position) {
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] |= bitBoard;
}

void Board::putKing(Color color, Square position) {
    hasPieceOfColor[color] &= ~(BitBoard(kingPosition[color]));
    kingPosition[color] = position;
    auto bitBoard = BitBoard(position);
    auto invBitBoard = ~bitBoard;
    setPieceColor(color, bitBoard);
    hasPieceOfType[PAWN] &= invBitBoard;
    hasPieceOfType[KNIGHT] &= invBitBoard;
    hasPieceOfType[BISHOP] &= invBitBoard;
    hasPieceOfType[ROOK] &= invBitBoard;
    hasPieceOfType[QUEEN] &= invBitBoard;
}

void Board::setPieceColor(Color color, BitBoard bitBoard) {
    hasPieceOfColor[color] |= bitBoard;
    hasPieceOfColor[opposite(color)] &= ~bitBoard;
}

BitBoard Board::getAttacksFrom(Color color) const {
    return attacks[color];
}

void Board::addRookPossibleMoves(BitBoard origin, std::vector<Move> & moves) {
    Color attackerColor = hasPieceOfColor[BLACK][origin] ? BLACK : WHITE;
    Color oppositeColor = opposite(attackerColor);

    BitBoard square = origin;
    while (!rank8[square]) {
        square <<= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());

        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!rank1[square]) {
        square >>= 8;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileA[square]) {
        square <<= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }

    square = origin;
    while (!fileH[square]) {
        square >>= 1;
        if (hasPieceOfColor[attackerColor][square]) break;
        moves.emplace_back(origin.asSquarePosition(), square.asSquarePosition());
        if (hasPieceOfColor[oppositeColor][square]) break;
    }
}

void Board::addKingPossibleMoves(BitBoard origin, std::vector<Move>& moves) {


}

Board::Board(const string& fen) {

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

