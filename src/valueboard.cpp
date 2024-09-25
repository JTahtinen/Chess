#include "defs.h"
#include "valueboard.h"

void ValueBoard::setValue(int column, int row, int value)
{
    if (!isValidSquare(column, row))
    {
        DEBUGERR("Tried to set value of invalid square: column %d, row %d, value %d\n", column, row, value);
        return;
    }
    this->squares[(column - 1) + (row - 1) * NUM_COLUMNS] = value;
}

void ValueBoard::setValue(Square square, int value)
{
    this->setValue(square.column, square.row, value);
}

void ValueBoard::increaseValue(int column, int row)
{
    setValue(column, row, getSquareValue(column, row) + 1);
}

void ValueBoard::increaseValue(Square square)
{
    this->increaseValue(square.column, square.row);
}

void ValueBoard::decreaseValue(int column, int row)
{
    setValue(column, row, getSquareValue(column, row) - 1);
}

void ValueBoard::decreaseValue(Square square)
{
    this->decreaseValue(square.column, square.row);
}

int ValueBoard::getSquareValue(int column, int row)
{
    if (!isValidSquare(column, row))
    {
        DEBUGERR("Tried to get value of invalid square: column %d, row %d\n", column, row);
        return 0;
    }
    return this->squares[(column - 1) + (row - 1) * NUM_COLUMNS];
}

int ValueBoard::getSquareValue(Square square)
{
    int result = getSquareValue(square.column, square.row);
    return result;
}

void ValueBoard::clear()
{
    for (int i = 0; i < NUM_COLUMNS * NUM_ROWS; ++i)
    {
        this->squares[i] = 0;
    }
    // memset(this->squares, 0, NUM_COLUMNS * NUM_ROWS * sizeof(int));
}
