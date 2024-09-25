#pragma once

#define NUM_COLUMNS (8)
#define NUM_ROWS (8)

enum Column
{
    NULL_COLUMN = 0,
    COL_A,
    COL_B,
    COL_C,
    COL_D,
    COL_E,
    COL_F,
    COL_G,
    COL_H,
};

enum Row
{
    NULL_ROW = 0,
    ROW_1,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,
    ROW_7,
    ROW_8
};

struct Square
{
    int column;
    int row;

    Square();
    Square(int column, int row);
    bool isSquare(int column, int row) const;
    bool operator==(const Square &other) const;
    bool operator!=(const Square &other) const;
};

bool isValidSquare(Square square);
bool isValidSquare(int column, int row);
