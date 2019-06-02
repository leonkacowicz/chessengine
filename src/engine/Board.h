#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "BitBoard.h"
#include "Square.h"
#include "Move.h"
#include <Piece.h>
#include <Color.h>

class Board {
    BitBoard hasPieceOfColor[2];
    BitBoard hasPieceOfType[5];
    BitBoard attacks[2];
    Square kingPosition[2];
    
    char enPassantAndCastles; // justMovedPawn2Squres [1-bit] + enPassantFile [4-bits] + whiteCanCastleKingSide
    bool justMovedPawn2Squares[2];
    bool castleKingSideAllowedFor[2];
    bool sideToPlay;

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

    void putPiece(Piece piece, Color color, Square position);

    void putPawn(Color color, Square position);
    void putKight(Color color, Square position);
    void putBishop(Color color, Square position);
    void putRook(Color color, Square position);
    void putQueen(Color color, Square position);
    void putKing(Color color, Square position);

    void setPieceColor(Color color, BitBoard bitBoard);

    BitBoard getAttacksFrom(Color color) const;

    void calculateKingAttacks(const BitBoard origin);

    std::vector<Move> getPossibleMovesFor(Color color);

    void addRookPossibleMoves(BitBoard board, std::vector<Move> &moves);

    void addKingPossibleMoves(BitBoard origin, vector<Move> &moves);

    vector<Move> pseudo_legal_rook_moves(const BitBoard origin, Color attackerColor) const noexcept;
};


#endif //CHESSENGINE_BOARD_H
