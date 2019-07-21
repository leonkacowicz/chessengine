#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "BitBoard.h"
#include "Square.h"
#include "Move.h"
#include <Piece.h>
#include "../chess/color.h"

class Board {
    BitBoard hasPieceOfColor[2];
    BitBoard hasPieceOfType[5];
    BitBoard attacks[2];
    Square kingPosition[2];
    
    char enPassantAndCastles; // justMovedPawn2Squres [1-bit] + enPassantFile [4-bits] + whiteCanCastleKingSide
    bool justMovedPawn2Squares[2];
    bool castleKingSideAllowedFor[2];
    bool sideToPlay;

    void addRookPossibleMoves(BitBoard board, std::vector<Move> &moves);
    void addKingPossibleMoves(BitBoard origin, vector<Move> &moves);
    vector<Move> pseudo_legal_rook_moves(const BitBoard origin, color attackerColor) const noexcept;
public:

    Board() :
        hasPieceOfColor{0, 0},
        hasPieceOfType{0, 0, 0, 0, 0}
    {
        putKing(WHITE, Square("e1"));
        putKing(BLACK, Square("e8"));
        calculateAttacks();
    }

    Board(const string& fen);

    bool isCheckmate() const;
    void setInitialPosition();
    void calculateAttacks();
    std::string toString() const;
    void printBoard() const;

    void calculateRookAttacks(BitBoard origin);

    void putPiece(Piece piece, color color, Square position);

    void putPawn(color color, Square position);
    void putKight(color color, Square position);
    void putBishop(color color, Square position);
    void putRook(color color, Square position);
    void putQueen(color color, Square position);
    void putKing(color color, Square position);

    void setPieceColor(color color, BitBoard bitBoard);

    BitBoard getAttacksFrom(color color) const;

    void calculateKingAttacks(const BitBoard origin);

    std::vector<Move> getPossibleMovesFor(color color);

};


#endif //CHESSENGINE_BOARD_H
