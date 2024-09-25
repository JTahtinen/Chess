#pragma once
#include "game.h"

enum KnightMoveType
{
    KNIGHT_MOVE_LEGAL,
    KNIGHT_MOVE_ILLEGAL,
};

void calculateKnightControlSquares(int column, int row, Side side, Square *squares, int *numSquares);

KnightMoveType getKnightMoveType(int  startColumn, int startRow, int endColumn, int endRow, Side side);