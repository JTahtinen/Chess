#include "piece.h"

Piece createPiece(Side side, PieceType type, int initColumn, int initRow)
{
    Piece result;
    result.square = Square(initColumn, initRow);
    result.side = side;
    result.type = type;
    result.alive = true;
    result.movementSquares.clear();
    return result;
}