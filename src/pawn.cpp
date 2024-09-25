#include "pawn.h"

bool isPawnStartingRow(int row, Side side)
{
    bool result = (side == SIDE_WHITE && row == ROW_2) || (side == SIDE_BLACK && row == ROW_7);
    return result;
}
