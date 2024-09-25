#pragma once
#include "game.h"

enum KingMoveType
{
    KING_MOVE_LEGAL,
    KING_MOVE_ILLEGAL,
};

void calculateKingControlSquares(int column, int row, Side side, Square *squares, int *numSquares);

KingMoveType getKingMoveType(int  startColumn, int startRow, int endColumn, int endRow, Side side);