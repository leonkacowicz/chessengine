#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "bitboard.h"
#include "square.h"
#include "Move.h"
#include "piece.h"
#include "color.h"

class Board {
    bitboard hasPieceOfColor[2];
    bitboard hasPieceOfType[5];
    bitboard attacks[2];
    square kingPosition[2];
    
    char enPassantAndCastles; // justMovedPawn2Squres [1-bit] + enPassantFile [4-bits] + whiteCanCastleKingSide
    bool justMovedPawn2Squares[2];
    bool castleKingSideAllowedFor[2];
    bool sideToPlay;

    void addRookPossibleMoves(bitboard board, std::vector<Move> &moves);
    void addKingPossibleMoves(bitboard origin, std::vector<Move> &moves);
    std::vector<Move> pseudo_legal_rook_moves(const bitboard origin, color attackerColor) const noexcept;
public:

    Board() :
        hasPieceOfColor{0, 0},
        hasPieceOfType{0, 0, 0, 0, 0}
    {
        putKing(WHITE, square("e1"));
        putKing(BLACK, square("e8"));
        calculateAttacks();
    }

    Board(const std::string& fen);

    bool isCheckmate() const;
    void setInitialPosition();
    void calculateAttacks();
    std::string toString() const;
    void printBoard() const;

    void calculateRookAttacks(bitboard origin);

    void putPiece(piece p, color c, square s);

    void putKing(color c, square position);

    void setPieceColor(color color, bitboard bitBoard);

    bitboard getAttacksFrom(color color) const;

    void calculateKingAttacks(const bitboard origin);

    std::vector<Move> getPossibleMovesFor(color color);

};


#endif //CHESSENGINE_BOARD_H
