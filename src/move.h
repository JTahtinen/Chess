#pragma once
#include "game.h"

void calculateControlSquaresForPiece(Piece *piece);

void calculateMovementSquaresForPiece(Piece *piece);

bool tryToMove(Piece *piece, Square square);
