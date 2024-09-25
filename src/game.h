#pragma once
#include <jadel.h>
#include "square.h"
#include "piece.h"
#include "valueboard.h"
#include "side.h"
#include "clickable.h"

#define MAX_PIECES (32)
#define MAX_BUTTONS (10)

struct WorldRect
{
    jadel::Vec2 worldStart;
    jadel::Vec2 worldEnd;
    jadel::Vec2 worldCenter;
    bool valid;
};

struct Player
{
    ChessSideInfo side;
    Piece pieces[MAX_PIECES];
    int numPieces;
    Piece *king;

    bool shortCastleRights;
    bool longCastleRights;
    ValueBoard controlSquares;
};

extern bool menuState;
extern Clickable menuButtons[MAX_BUTTONS];
extern int numMenuButtons;

extern jadel::Rectf pieceRect;
extern jadel::Rectf squareRect;
extern float squareSideDiameter;
extern float pieceDim;
extern float halfPieceDim;
extern jadel::Vec2 boardStart;

extern Player players[2];
extern Player *player;
extern int turn;

extern Piece *alivePieces[MAX_PIECES];
extern uint32 numAlivePieces;
extern Piece *selectedPiece;
extern Piece *grabbedPiece;

extern Square highlightedSquare;
extern bool squareIsHighlighted;


Piece *getPieceFromSquare(Square square);

Piece *getPieceFromSquare(int column, int row);

Square getNullSquare();

void killPiece(Piece *piece);

void submitForPromotion(Piece *piece);

bool isValidSquare(Square square);

bool isValidSquare(int column, int row);

bool isWhiteOccupiedSquare(Square square);

bool isWhiteOccupiedSquare(int column, int row);

bool isBlackOccupiedSquare(Square square);

bool isBlackOccupiedSquare(int column, int row);

bool isOccupiedSquareBy(Square square, int side);

bool isOccupiedSquareBy(int column, int row, int side);

bool isOccupiedSquare(Square square);

bool isOccupiedSquare(int column, int row);

WorldRect getSquarePos(Square square);

void startGame();

void initGame();

void updateGame();

void gameRender();