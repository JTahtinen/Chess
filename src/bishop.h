#pragma once
#include "game.h"

enum BishopMoveType
{
    BISHOP_MOVE_LEGAL,
    BISHOP_MOVE_ILLEGAL
};

BishopMoveType getBishopMoveType(int startColumn, int startRow, int endColumn, int endRow, Side side);