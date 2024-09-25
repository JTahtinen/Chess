#include "game.h"
#include "gamerender.h"
#include "math.h"
#include "move.h"
#include "defs.h"
#include "clickable.h"
#include "application.h"
#include <jadel.h>

jadel::Vec2 boardStart;

Square nullSquare{0, 0};

static bool moved;

jadel::Rectf pieceRect;
jadel::Rectf squareRect;
float squareSideDiameter;

float pieceDim;
float halfPieceDim;

Piece *alivePieces[MAX_PIECES];
uint32 numAlivePieces = 2;

Player players[2];
Player *player;
int turn;

Piece *grabbedPiece = NULL;
Piece *selectedPiece = NULL;
Square highlightedSquare;
bool squareIsHighlighted;
int whitePieceScore;
int blackPieceScore;

Piece *pawnToPromote;

Clickable menuButtons[MAX_BUTTONS];
int numMenuButtons = 0;

static Clickable newGameButton;
static Clickable quitButton;

bool menuState = false;
// Clickable clickableBoard[64];

Square getSquareOfPiece(Piece *piece)
{
    if (!piece)
    {
        return nullSquare;
    }
    Square result = piece->square;
    return result;
}

Square getNullSquare()
{
    return nullSquare;
}

Piece *getPieceFromSquare(Square square)
{
    for (uint32 i = 0; i < numAlivePieces; ++i)
    {
        if (alivePieces[i]->square == square)
        {
            return alivePieces[i];
        }
    }
    return NULL;
}

Piece *getPieceFromSquare(int column, int row)
{
    Piece *result = getPieceFromSquare(Square(column, row));
    return result;
}

static void calculateMovementSquares()
{
    calculateMovementSquaresForPiece(player->king);
    FOR_EX_U(i, 2, numAlivePieces)
    {
        Piece *piece = alivePieces[i];
        if (piece->side == turn)
        {
            calculateMovementSquaresForPiece(piece);
        }
    }
}

static void calculateControlSquares()
{
    players[SIDE_WHITE].controlSquares.clear();
    players[SIDE_BLACK].controlSquares.clear();

    for (uint32 i = 0; i < numAlivePieces; ++i)
    {
        Piece *piece = alivePieces[i];
        calculateControlSquaresForPiece(piece);
    }
}

static int getTotalMaterialValue()
{
    int result = whitePieceScore - blackPieceScore;
    return result;
}

static int getPieceValue(const Piece *piece)
{
    if (!piece)
    {
        return 0;
    }
    int result;
    switch (piece->type)
    {
    case PIECE_PAWN:
    {
        result = 1;
        break;
    }
    case PIECE_KNIGHT:
    case PIECE_BISHOP:
    {
        result = 3;
        break;
    }
    case PIECE_ROOK:
    {
        result = 5;
        break;
    }
    case PIECE_QUEEN:
    {
        result = 9;
        break;
    }
    default:
    {
        result = 0;
        break;
    }
    }
    return result;
}

static void increaseMaterialPoints(const Piece *piece)
{
    if (!piece)
    {
        return;
    }
    if (piece->side == SIDE_WHITE)
    {
        whitePieceScore += getPieceValue(piece);
    }
    else
    {
        blackPieceScore += getPieceValue(piece);
    }
}

static void decreaseMaterialPoints(const Piece *piece)
{
    if (!piece)
    {
        return;
    }
    if (piece->side == SIDE_WHITE)
    {
        whitePieceScore -= getPieceValue(piece);
    }
    else
    {
        blackPieceScore -= getPieceValue(piece);
    }
}

static void calculateMaterialPoints()
{
    whitePieceScore = 0;
    blackPieceScore = 0;
    FOR_EX_I(i, 0, numAlivePieces)
    {
        const Piece *piece = alivePieces[i];
        increaseMaterialPoints(piece);
    }
}

static void selectPiece(Piece *piece)
{
    selectedPiece = piece;
}

static void pushAlivePiece(Piece *piece)
{
    if (numAlivePieces >= MAX_PIECES)
    {
        DEBUGERR("Too many alive pieces!\n");
        return;
    }
    alivePieces[numAlivePieces++] = piece;
}

static void recalculateAlivePieces()
{
    numAlivePieces = 2;
    FOR_EX_I(p, 2)
    {
        FOR_EX_I(i, 1, MAX_PIECES)
        {
            Piece *piece = &players[p].pieces[i];
            if (piece->alive)
            {
                pushAlivePiece(piece);
            }
        }
    }
}

void killPiece(Piece *piece)
{
    if (!piece)
    {
        jadel::message("[ERROR] tried to kill nonexistent piece!");
    }
    piece->alive = false;
    decreaseMaterialPoints(piece);
    recalculateAlivePieces();
    int totalScore = getTotalMaterialValue();
    if (totalScore > 0)
    {
        jadel::message("White material value: %d\n", totalScore);
    }
    else if (totalScore < 0)
    {
        jadel::message("Black material value: %d\n", -totalScore);
    }
    else
    {
        jadel::message("Material value: %d\n", 0);
    }
}

void submitForPromotion(Piece *piece)
{
    pawnToPromote = piece;
}

static void startTurn()
{
    moved = false;
    calculateControlSquares();
    calculateMovementSquares();
}

static void endTurn()
{
    turn = !turn;
    player = &players[turn];
    selectedPiece = NULL;
    startTurn();
}

bool isOccupiedSquare(Square square, Piece *piecesToCheck)
{
    if (!isValidSquare(square))
    {
        return false;
    }
    bool result = false;
    for (uint32 i = 0; i < numAlivePieces; ++i)
    {
        Piece *piece = &piecesToCheck[i];
        if (piece->alive && piece->square == square)
        {
            result = true;
            break;
        }
    }
    return result;
}

bool isOccupiedSquare(int column, int row, Piece *piecesToCheck)
{
    bool result = isOccupiedSquare(Square(column, row), piecesToCheck);
    return result;
}

bool isWhiteOccupiedSquare(Square square)
{
    bool result = isOccupiedSquare(square, players[SIDE_WHITE].pieces);
    return result;
}

bool isWhiteOccupiedSquare(int column, int row)
{
    bool result = isWhiteOccupiedSquare(Square(column, row));
    return result;
}

bool isBlackOccupiedSquare(Square square)
{
    bool result = isOccupiedSquare(square, players[SIDE_BLACK].pieces);
    return result;
}

bool isBlackOccupiedSquare(int column, int row)
{
    bool result = isBlackOccupiedSquare(Square(column, row));
    return result;
}

bool isOccupiedSquareBy(Square square, int side)
{
    if (side != SIDE_WHITE && side != SIDE_BLACK)
    {
        return false;
    }
    bool result = side == SIDE_WHITE ? isWhiteOccupiedSquare(square) : isBlackOccupiedSquare(square);
    return result;
}

bool isOccupiedSquareBy(int column, int row, int side)
{
    bool result = isOccupiedSquareBy(Square(column, row), side);
    return result;
}

bool isOccupiedSquare(Square square)
{
    bool result = (isWhiteOccupiedSquare(square) || isBlackOccupiedSquare(square));
    return result;
}

bool isOccupiedSquare(int column, int row)
{
    bool result = isOccupiedSquare(Square(column, row));
    return result;
}

WorldRect getSquarePos(Square square)
{
    int column = square.column;
    int row = square.row;
    if (column < COL_A || column > NUM_COLUMNS || row < ROW_1 || row > NUM_ROWS)
    {
        jadel::message("[ERROR] Tried to get dimensions for nonexistent square: column %d, row %d\n", column, row);
        WorldRect invalidSquare{};
        invalidSquare.valid = false;
        return invalidSquare;
    }
    WorldRect result;
    result.worldStart = jadel::Vec2(boardStart.x - (NUM_COLUMNS / 2) * squareSideDiameter + (column - 1) * squareSideDiameter,
                                    boardStart.x - (NUM_ROWS / 2) * squareSideDiameter + (row - 1) * squareSideDiameter);
    result.worldEnd = result.worldStart + jadel::Vec2(squareSideDiameter, squareSideDiameter);
    result.worldCenter = result.worldStart + squareRect.getPointB();
    result.valid = true;
    return result;
}

Clickable whitePromotionButtons[4];
Clickable blackPromotionButtons[4];

static jadel::Point2i getMouseHoverSquare()
{
    jadel::Vec2 mousePos = getMousePos();
    jadel::Rectf projBoardDim = getProjectedBoardScreenDim();
    jadel::Vec2 projBoardStart = projBoardDim.getPointA();
    jadel::Vec2 projBoardEnd = projBoardDim.getPointB();
    if (mousePos.x < projBoardStart.x || mousePos.x >= projBoardEnd.x || mousePos.y < projBoardStart.y || mousePos.y >= projBoardEnd.y)
    {
        return jadel::Point2i{-1, -1};
    }

    jadel::Vec2 boardSize = projBoardEnd - projBoardStart;
    jadel::Vec2 boardRelativeMousePos = mousePos - projBoardStart;
    boardRelativeMousePos.x /= boardSize.x;
    boardRelativeMousePos.y /= boardSize.y;
    int mouseCol = (int)(boardRelativeMousePos.x * (float)NUM_COLUMNS) + 1;
    int mouseRow = (int)(boardRelativeMousePos.y * (float)NUM_ROWS) + 1;
    jadel::Point2i result{mouseCol, mouseRow};
    return result;
}

static jadel::Vec2 savedMousePos;
static bool promotionButtonPositionDetermined = false;
bool askPromotion()
{
    jadel::Rectf projectedSquareRect = getProjectedQuareScreenDim();
    jadel::Vec2 projectedSquare(projectedSquareRect.x1 * 2.0f, projectedSquareRect.y1 * 2.0f);
    Clickable *promotionButtons = turn == SIDE_WHITE ? whitePromotionButtons : blackPromotionButtons;
    float margin = 0.1f * projectedSquare.x;
    for (int i = 0; i < 4; ++i)
    {
        if (!promotionButtonPositionDetermined)
        {
            jadel::Vec2 screenElemPos = savedMousePos + jadel::Vec2(-1.5f * projectedSquare.x + (i * margin) + i * projectedSquare.x, 0.5f * projectedSquare.y + margin);
            promotionButtons[i].setPos(screenElemPos);
        }
        promotionButtons[i].update();
        promotionButtons[i].render();

        if (promotionButtons[i].isClicked())
        {
            switch (i)
            {
            case 0:
                pawnToPromote->type = PIECE_QUEEN;
                break;
            case 1:
                pawnToPromote->type = PIECE_ROOK;
                break;
            case 2:
                pawnToPromote->type = PIECE_BISHOP;
                break;
            case 3:
                pawnToPromote->type = PIECE_KNIGHT;
                break;
            }
            pawnToPromote = NULL;
            promotionButtonPositionDetermined = false;
            return true;
        }
    }
    promotionButtonPositionDetermined = true;
    return false;
}

void toggleMenu()
{
    menuState = !menuState;
    if (menuState)
    {
        prepareMenuRender();
    }
}

void updateGame()
{
    if (jadel::inputIsKeyTyped(jadel::KEY_ESCAPE))
    {
        toggleMenu();
    }
    jadel::Vec2 mousePos = getMousePos();
    if (!menuState)
    {

        if (!moved)
        {
            promotionButtonPositionDetermined = false;
            jadel::Point2i mouseHoverSquare = getMouseHoverSquare();
            if (mouseHoverSquare.x == -1)
            {
                squareIsHighlighted = false;
            }
            else
            {
                highlightedSquare = Square{mouseHoverSquare.x, mouseHoverSquare.y};
                squareIsHighlighted = true;
                if (jadel::inputIsMouseLeftClicked())
                {
                    FOR_EX_I(i, numAlivePieces)
                    {
                        Piece *piece = alivePieces[i];
                        if (piece->square == highlightedSquare)
                        {
                            selectPiece(piece);
                            grabbedPiece = piece;
                            break;
                        }
                    }
                }
            }
            if (jadel::inputIsMouseLeftReleased())
            {
                if (grabbedPiece)
                {
                    if (squareIsHighlighted)
                    {
                        moved = tryToMove(grabbedPiece, highlightedSquare);
                        savedMousePos = mousePos;
                    }
                    grabbedPiece = NULL;
                }
            }
        }
        if (moved)
        {
            if (pawnToPromote)
            {
                if (askPromotion())
                {
                    endTurn();
                }
            }
            else
            {
                endTurn();
            }
        }
    }
    else
    {
        for (int i = 0; i < numMenuButtons; ++i)
        {
            menuButtons[i].update();
        }
    }
}

static Piece *addPiece(Player *player, PieceType type, int column, int row)
{
    if (player->numPieces >= MAX_PIECES)
    {
        DEBUGERR("Tried to add too many pieces for player.\n");
        return NULL;
    }
    Piece *piece = &player->pieces[player->numPieces++];
    *piece = createPiece(player->side.side, type, column, row);
    return piece;
}

static void setupPlayers()
{
    FOR_EX_I(p, 2)
    {
        Player *player = &players[p];
        player->numPieces = 0;
        FOR_EX_I(i, MAX_PIECES)
        {
            player->pieces[i].alive = false;
        }
        player->shortCastleRights = true;
        player->longCastleRights = true;
        player->controlSquares.clear();
        player->king = addPiece(player, PIECE_KING, COL_E, player->side.pieceRow);

        addPiece(player, PIECE_ROOK, COL_A, player->side.pieceRow);
        addPiece(player, PIECE_KNIGHT, COL_B, player->side.pieceRow);
        addPiece(player, PIECE_BISHOP, COL_C, player->side.pieceRow);
        addPiece(player, PIECE_QUEEN, COL_D, player->side.pieceRow);
        addPiece(player, PIECE_BISHOP, COL_F, player->side.pieceRow);
        addPiece(player, PIECE_KNIGHT, COL_G, player->side.pieceRow);
        addPiece(player, PIECE_ROOK, COL_H, player->side.pieceRow);

        FOR_IN_I(col, 1, 8)
        {
            addPiece(player, PIECE_PAWN, col, player->side.pawnRow);
        }
        // index 0 for white, 1 for black
        alivePieces[player->side.side] = player->king;
    }
}

static void setupGame()
{
    pawnToPromote = NULL;

    Piece *whitePieces = players[SIDE_WHITE].pieces;
    Piece *blackPieces = players[SIDE_BLACK].pieces;

    setupPlayers();

    recalculateAlivePieces();
    calculateMaterialPoints();

    turn = SIDE_WHITE;
    player = &players[turn];
}

static ChessSideInfo createChessSideInfo(Side side)
{
    int advDir = side == SIDE_WHITE ? 1 : -1;
    int pieceRow = side == SIDE_WHITE ? ROW_1 : ROW_8;
    int pawnRow = pieceRow + advDir;

    CastlingInfo shortCastlingInfo;
    shortCastlingInfo.kingEndSquare = Square(COL_G, pieceRow);
    shortCastlingInfo.rookStartSquare = Square(COL_H, pieceRow);
    shortCastlingInfo.rookEndSquare = Square(COL_F, pieceRow);

    CastlingInfo longCastlingInfo;
    longCastlingInfo.kingEndSquare = Square(COL_C, pieceRow);
    longCastlingInfo.rookStartSquare = Square(COL_A, pieceRow);
    longCastlingInfo.rookEndSquare = Square(COL_D, pieceRow);

    ChessSideInfo result;

    result.side = side;
    result.pieceRow = pieceRow;
    result.pawnRow = pawnRow;
    result.pawnAdvanceDirection = advDir;
    result.shortCastlingInfo = shortCastlingInfo;
    result.longCastlingInfo = longCastlingInfo;

    return result;
}

static void initPlayers()
{
    players[SIDE_WHITE].side = createChessSideInfo(SIDE_WHITE);
    players[SIDE_BLACK].side = createChessSideInfo(SIDE_BLACK);
}

static void initGameSpace()
{
    boardStart = jadel::Vec2(0, 0);
    float pieceWidth = 1.0f / 6.0f;
    float pieceHeight = 1.0f / 2.0f;
    // knight.texStartCoords = jadel::Vec2(pieceWidth * 3, 0.0f);
    // knight.texEndCoords = jadel::Vec2(pieceWidth * 4, 0.5f);

    squareRect = jadel::Rectf(-0.1f, -0.1f, 0.1f, 0.1f);
    squareSideDiameter = getRectDimensions(squareRect).x;

    pieceDim = squareRect.x1;
    halfPieceDim = pieceDim * 0.5f;
    pieceRect = squareRect;

    const Texture *whitePromotionTextures[4] = {&wQueen, &wRook, &wBishop, &wKnight};
    const Texture *blackPromotionTextures[4] = {&bQueen, &bRook, &bBishop, &bKnight};

    FOR_EX_I(i, 4)
    {
        whitePromotionButtons[i].setDimensions(squareRect);
        blackPromotionButtons[i].setDimensions(squareRect);
        whitePromotionButtons[i].setTexture(whitePromotionTextures[i]);
        blackPromotionButtons[i].setTexture(blackPromotionTextures[i]);
        whitePromotionButtons[i].addVisualFlags(CLICKABLE_VISUAL_COLORED | CLICKABLE_VISUAL_TEXTURED | CLICKABLE_VISUAL_HIGHLIGHT_HOVER);
        blackPromotionButtons[i].addVisualFlags(CLICKABLE_VISUAL_COLORED | CLICKABLE_VISUAL_TEXTURED | CLICKABLE_VISUAL_HIGHLIGHT_HOVER);
    }
}

static bool addMenuButton(void (*callback)())
{
    static float margin = 0.02f;
    if (numMenuButtons >= MAX_BUTTONS)
    {
        ERRMSG("Too many buttons\n");
        return false;
    }
    Clickable *button = &menuButtons[numMenuButtons++];

    *button = Clickable();
    button->setArea(-0.95, 1.0f - margin - numMenuButtons * (0.1f + margin), 0, 0, 0.4f, -0.1f);
    button->setCallBack(callback);
    return true;
}

static void quitGame()
{
    exit(0);
}

static void initGameUI()
{
    addMenuButton(toggleMenu);
    addMenuButton(startGame);
    addMenuButton(quitGame);
}

void initGame()
{
    initGameUI();
    initGameSpace();
    initPlayers();
}

void startGame()
{
    setupGame();
    startTurn();
}