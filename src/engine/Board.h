#ifndef CHESSENGINE_BOARD_H
#define CHESSENGINE_BOARD_H

#include <vector>
#include "BitBoard.h"
#include "SquarePosition.h"
#include "Move.h"
#include <Piece.h>
#include <Color.h>

class Board {
    BitBoard hasPieceOfColor[2];
    BitBoard hasPieceOfType[5];
    BitBoard attacks[2];
    SquarePosition kingPosition[2];
    char enPassantAndCastles; // justMovedPawn2Squres [1-bit] + enPassantFile [4-bits] + whiteCanCastleKingSide
    bool justMovedPawn2Squares[2];
    bool castleKingSideAllowedFor[2];
    bool sideToPlay;

public:

    Board() :
        hasPieceOfColor{0, 0},
        hasPieceOfType{0, 0, 0, 0, 0}
    {
        putKing(WHITE, SquarePosition("e1"));
        putKing(BLACK, SquarePosition("e8"));
        calculateAttacks();
    }

    bool isCheckmate() const;
    void setInitialPosition();
    void calculateAttacks();
    std::string toString() const;
    void printBoard() const;

    void calculateRookAttacks(BitBoard origin);

    void putPiece(Piece piece, Color color, SquarePosition position);

    void putPawn(Color color, SquarePosition position);
    void putKight(Color color, SquarePosition position);
    void putBishop(Color color, SquarePosition position);
    void putRook(Color color, SquarePosition position);
    void putQueen(Color color, SquarePosition position);
    void putKing(Color color, SquarePosition position);

    void setPieceColor(Color color, BitBoard bitBoard);

    BitBoard getAttacksFrom(Color color) const;

    void calculateKingAttacks(const BitBoard origin);

    std::vector<Move> getPossibleMovesFor(Color color);

    void addRookPossibleMoves(BitBoard board, std::vector<Move> &moves);
};


#endif //CHESSENGINE_BOARD_H
