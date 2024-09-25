#pragma once
#include "castle.h"

enum Side
{
    SIDE_WHITE = 0,
    SIDE_BLACK = 1
};

struct ChessSideInfo
{
    Side side;
    int pieceRow;
    int pawnRow;
    int pawnAdvanceDirection;
    CastlingInfo shortCastlingInfo;
    CastlingInfo longCastlingInfo;

    inline bool operator==(ChessSideInfo &other) const
    {
        bool result = this->side == other.side;
        return result;
    }

    inline bool operator!=(ChessSideInfo &other) const
    {
        bool result = !(*this == other);
        return result;
    }

    inline bool operator==(Side other) const
    {
        bool result = this->side == other;
        return result;
    }

        inline bool operator!=(Side other) const
    {
        bool result = !(*this == other);
        return result;
    }
};
