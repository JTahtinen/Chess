#pragma once
#include "square.h"
#include "valueboard.h"
#include "side.h"

enum PieceType
{
    PIECE_PAWN = 0,
    PIECE_BISHOP,
    PIECE_KNIGHT,
    PIECE_ROOK,
    PIECE_QUEEN,
    PIECE_KING,
    NUM_PIECE_TYPES
};

struct Piece
{
    Square square;
    PieceType type;
    Side side;
    bool alive;
    ValueBoard movementSquares;
};

Piece createPiece(Side side, PieceType type, int initColumn, int initRow);
