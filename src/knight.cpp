#include "knight.h"
#include "game.h"
#include "util.h"
#include <jadel.h>


KnightMoveType getKnightMoveType(int startColumn, int startRow, int endColumn, int endRow, Side side)
{
    if (isOccupiedSquareBy(endColumn, endRow, side))
    {
        return KNIGHT_MOVE_ILLEGAL;
    }
    int verticalMovement = jadel::absInt(endRow - startRow);
    int horizontalMovement = jadel::absInt(endColumn - startColumn);
    if ((verticalMovement == 2 && horizontalMovement == 1) || (verticalMovement == 1 && horizontalMovement == 2))
    {
        return KNIGHT_MOVE_LEGAL;
    }
    return KNIGHT_MOVE_ILLEGAL;
}