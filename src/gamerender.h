#pragma once
#include <jadel.h>
#include "graphics.h"

extern Texture wPawn;
extern Texture wKing;
extern Texture wQueen;
extern Texture wBishop;
extern Texture wKnight;
extern Texture wRook;
extern Texture bPawn;
extern Texture bKing;
extern Texture bQueen;
extern Texture bBishop;
extern Texture bKnight;
extern Texture bRook;

void gameRender();

void gameRenderInit();

void prepareMenuRender();

jadel::Rectf getProjectedBoardScreenDim();

jadel::Rectf getProjectedQuareScreenDim();

