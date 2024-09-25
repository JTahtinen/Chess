#pragma once
#include "square.h"

struct ValueBoard
{
    int squares[NUM_COLUMNS * NUM_ROWS];

    void setValue(int column, int row, int value);
    void setValue(Square square, int value);
    void increaseValue(int column, int row);
    void increaseValue(Square square);
    void decreaseValue(int column, int row);
    void decreaseValue(Square square);
    int getSquareValue(int column, int row);
    int getSquareValue(Square square);
    void clear();
};