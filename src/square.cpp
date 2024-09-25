#include "square.h"

Square::Square(int column, int row)
    : column(column), row(row)
{
}
Square::Square()
    : Square(0, 0)
{
}

bool Square::isSquare(int column, int row) const
{
    bool result = this->column == column && this->row == row;
    return result;
}

bool Square::operator==(const Square &other) const
{
    bool result = this->isSquare(other.column, other.row);
    return result;
}

bool Square::operator!=(const Square &other) const
{
    bool result = !(*this == other);
    return result;
}


bool isValidSquare(Square square)
{
    bool result = isValidSquare(square.column, square.row);
    return result;
}

bool isValidSquare(int column, int row)
{
    bool result = (column >= COL_A && column <= NUM_COLUMNS && row >= ROW_1 && row <= NUM_ROWS);
    return result;
}