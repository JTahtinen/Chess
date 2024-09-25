#include <jadel.h>
#include "game.h"
#include "defs.h"
#include "render.h"

#define MAX_SURFACES (50)

static jadel::Surface pieceAtlas;

Texture wPawn;
Texture wKing;
Texture wQueen;
Texture wBishop;
Texture wKnight;
Texture wRook;
Texture bPawn;
Texture bKing;
Texture bQueen;
Texture bBishop;
Texture bKnight;
Texture bRook;

static Texture circleTex;
static Texture circlePurpleTex;
static Texture boardTex;
static Texture menuTex;

static jadel::Surface surfaces[MAX_SURFACES];
static int numSurfaces;
static uint32 lightSquareColor = 0xff887878;
static uint32 darkSquareColor = 0xff771111;

uint32 createSurface(int width, int height)
{
    if (numSurfaces >= MAX_SURFACES)
    {
        ERRMSG("Could not create surface: - Too many surfaces\n");
        return 0;
    }
    if (!jadel::graphicsCreateSurface(width, height, &surfaces[numSurfaces]))
    {
        return 0;
    }
    int handle = numSurfaces + 1;
    ++numSurfaces;
    return handle;
}

uint32 loadSurface(const char *filename)
{
    if (numSurfaces >= 10)
    {
        ERRMSG("Could not load surface: %s - Too many surfaces\n", filename);
        return 0;
    }
    if (!jadel::loadPNG(filename, &surfaces[numSurfaces]))
    {
        ERRMSG("Could not load surface: %s\n", filename);
        return 0;
    }
    int handle = numSurfaces + 1;
    ++numSurfaces;
    return handle;
}

jadel::Surface *getSurface(uint32 handle)
{
    if (handle == 0 || handle > numSurfaces)
    {
        ERRMSG("Invalid surface handle: %d\n", handle);
        return &errorSurface;
    }
    jadel::Surface *result = &surfaces[handle - 1];
    return result;
}

Texture createRenderTexture(jadel::Surface *surface)
{
    if (!surface)
    {
        ERRMSG("Could not create texture: NULL Surface!\n");
        return errorTexture;
    }
    Texture result = graphicsCreateTexture(jadel::Vec2(0, 0), jadel::Vec2(1, 0), surface);
    return result;
}

Texture createRenderTexture(float coordAX, float coordAY, float coordBX, float coordBY, jadel::Surface *surface)
{
    if (!surface)
    {
        ERRMSG("Could not load Texture!\n");
        return Texture{};
    }
    int finalCoordAX, finalCoordAY, finalCoordBX, finalCoordBY;
    if (surface == &errorSurface)
    {
        return errorTexture;
    }
    Texture result = graphicsCreateTexture(jadel::Vec2(coordAX, coordAY), jadel::Vec2(coordBX, coordBY), surface);
    return result;
}

Texture createRenderTexture(float coordAX, float coordAY, float coordBX, float coordBY, int handle)
{
    jadel::Surface *surface = getSurface(handle);
    Texture result = createRenderTexture(coordAX, coordAY, coordBX, coordBY, surface);
    return result;
}

bool createTexturesFromAtlas(uint32 surfaceHandle, int numHorizontalTextures, int numVerticalTextures, Texture **orderedTextureList)
{
    jadel::Surface *surface = getSurface(surfaceHandle);
    if (!surface)
    {
        DEBUGERR("Could not find texture atlas with handle %d\n", surfaceHandle);
        return false;
    }
    float texWidth = 1.0f / (float)numHorizontalTextures;
    float texHeight = 1.0f / (float)numVerticalTextures;
    for (int y = 0; y < numVerticalTextures; ++y)
    {
        float yStart = y * texHeight;
        float yEnd = yStart + texHeight;
        for (int x = 0; x < numHorizontalTextures; ++x)
        {
            float xStart = x * texWidth;
            float xEnd = xStart + texWidth;
            *orderedTextureList[x + y * numHorizontalTextures] = createRenderTexture(xStart, yStart, xEnd, yEnd, surface);
        }
    }
    return true;
}
int menuHandle;
void gameRenderInit()
{
    numSurfaces = 0;
    int pieceAtlasHandle = loadSurface("res/textures/pieces.png");
    int circleHandle = loadSurface("res/textures/circleSmall.png");
    int circlePurpleHandle = loadSurface("res/textures/circleSmallPurple.png");
    int boardHandle = loadSurface("res/textures/board.png");
    menuHandle = createSurface(winSurface.width, winSurface.height);
    circleTex = createRenderTexture(0, 0, 1, 1, circleHandle);
    circlePurpleTex = createRenderTexture(0, 0, 1, 1, circlePurpleHandle);
    boardTex = createRenderTexture(0, 0, 1, 1, boardHandle);
    menuTex = createRenderTexture(0, 0, 1, 1, menuHandle);

    float texPieceWidth = 1.0f / 6.0f;
    float texPieceHeight = 1.0f / 2.0f;
    Texture *pieceTextureList[] = {&bKing, &bQueen, &bBishop, &bKnight, &bRook, &bPawn, &wKing, &wQueen, &wBishop, &wKnight, &wRook, &wPawn};
    createTexturesFromAtlas(pieceAtlasHandle, 6, 2, pieceTextureList);
}

void renderRectOnSquare(float diameter, Square square, uint32 color)
{
    WorldRect screenSquare = getSquarePos(square);
    if (!screenSquare.valid)
    {
        return;
    }
    float halfDiameter = diameter * 0.5f;
    renderSubmitWorldRect(jadel::Rectf(-halfDiameter, -halfDiameter, halfDiameter, halfDiameter), screenSquare.worldCenter, color);
}

void renderTextureOnSquare(const Texture *texture, Square square, float sizeMod)
{
    WorldRect screenSquare = getSquarePos(square);
    if (!screenSquare.valid)
    {
        return;
    }
    jadel::Rectf rect(pieceRect.getPointA() * sizeMod, pieceRect.getPointB() * sizeMod);

    renderSubmitWorldTexture(rect, screenSquare.worldCenter, texture);
}

void renderTextureOnSquare(const Texture *texture, Square square)
{
    renderTextureOnSquare(texture, square, 1.0f);
}

void renderSquareHighlight(Square square)
{
    WorldRect squareWorldRect = getSquarePos(square);
    if (!squareWorldRect.valid)
    {
        jadel::message("[WARNING] Tried to highlight invalid square: column %d, row %d\n", square.column, square.row);
        return;
    }
    float highlightThickness = squareSideDiameter * 0.05f;
    renderSubmitWorldRect(squareRect, squareWorldRect.worldCenter, 0x77ffff03);
    /*
            renderRect(square.screenStart.x, square.screenStart.y,
                       square.screenStart.x + highlightThickness, square.screenEnd.y, 0xffffff00);
            renderRect(square.screenStart.x + highlightThickness, square.screenEnd.y - highlightThickness,
                       square.screenEnd.x, square.screenEnd.y, 0xffffff00);
            renderRect(square.screenEnd.x - highlightThickness, square.screenEnd.y - highlightThickness,
                       square.screenEnd.x, square.screenStart.y, 0xffffff00);
            renderRect(square.screenStart.x + highlightThickness, square.screenStart.y,
                       square.screenEnd.x - highlightThickness, square.screenStart.y + highlightThickness, 0xffffff00);
        */
}

const Texture *getPieceTexture(const Piece *piece)
{
    const Texture *pieceTexture;
    switch (piece->type)
    {
    case PIECE_KING:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wKing : &bKing;
        break;
    }
    case PIECE_QUEEN:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wQueen : &bQueen;
        break;
    }
    case PIECE_ROOK:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wRook : &bRook;
        break;
    }
    case PIECE_BISHOP:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wBishop : &bBishop;
        break;
    }
    case PIECE_KNIGHT:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wKnight : &bKnight;
        break;
    }
    case PIECE_PAWN:
    {
        pieceTexture = piece->side == SIDE_WHITE ? &wPawn : &bPawn;
        break;
    }
    default:
    {
        pieceTexture = NULL;
        break;
    }
    }
    return pieceTexture;
}

void renderEffects()
{
    static float movementIndicatorDiameter = squareSideDiameter * 0.5;
    if (selectedPiece && selectedPiece->side == turn)
    {
        for (int row = ROW_1; row <= NUM_ROWS; ++row)
        {
            for (int col = COL_A; col <= NUM_COLUMNS; ++col)
            {
                if (selectedPiece->movementSquares.getSquareValue(col, row) > 0)
                {
                    const Texture *circle;
                    if (isOccupiedSquareBy(col, row, !turn))
                    {
                        circle = &circlePurpleTex;
                    }
                    else
                    {
                        circle = &circleTex;
                    }
                    renderTextureOnSquare(circle, Square(col, row), 0.5f);
                }
            }
        }
    }
}

void renderMenu()
{
    static jadel::Mat3 identity = jadel::Mat3::identity();
    renderSubmitTextureRenderable(jadel::Rectf(-1.0f, -1.0f, 1.0f, 1.0f), &menuTex, identity);
    for (int i = 0; i < numMenuButtons; ++i)
    {
        menuButtons[i].render();
    }
}

void renderPieces()
{
    jadel::Vec2 mousePos(jadel::inputGetMouseXRelative(), jadel::inputGetMouseYRelative());

    for (uint32 i = 0; i < numAlivePieces; ++i)
    {
        Piece *piece = alivePieces[i];
        if (piece == grabbedPiece || !isValidSquare(piece->square))
        {
            continue;
        }
        const Texture *pieceTexture = getPieceTexture(piece);
        if (!pieceTexture)
        {
            jadel::message("[ERROR] Null piece texture!\n");
            return;
        }
        renderTextureOnSquare(pieceTexture, piece->square);
    }
    if (squareIsHighlighted)
    {
        renderSquareHighlight(highlightedSquare);
    }
    if (grabbedPiece)
    {
        renderSubmitScreenTexture(pieceRect, mousePos, getPieceTexture(grabbedPiece));
    }
}

void prepareMenuRender()
{
    jadel::Surface *menuSurface = getSurface(menuHandle);
    graphicsCreateBWSurfaceFromEqualSurface(&winSurface, menuSurface);
}

void renderBoard()
{
    static jadel::Rectf boardRect = squareRect * 8;
    bool currentFilterMode = graphicsGetFilterMode();
    graphicsSetFilterMode(false);
    renderSubmitWorldTexture(boardRect, jadel::Vec2(0, 0), &boardTex);
    graphicsSetFilterMode(currentFilterMode);
}

void gameRender()
{
    renderClear();
    if (menuState)
    {
        renderMenu();
    }
    else
    {
        renderBoard();
        renderPieces();
        renderEffects();
    }
    renderFlush();
}

jadel::Rectf getProjectedBoardScreenDim()
{
    jadel::Vec2 worldBoardStart = jadel::Vec2(boardStart.x - (NUM_COLUMNS / 2) * squareSideDiameter,
                                              boardStart.y - (NUM_ROWS / 2) * squareSideDiameter);
    jadel::Vec2 worldBoardEnd = worldBoardStart + (jadel::Vec2(NUM_COLUMNS, NUM_ROWS) * squareSideDiameter);
    jadel::Rectf result(projMatrix * worldBoardStart, projMatrix * worldBoardEnd);
    return result;
}

jadel::Rectf getProjectedQuareScreenDim()
{
    float halfSquareRadius = squareSideDiameter * 0.5f;
    jadel::Rectf result = projMatrix * jadel::Rectf(-halfSquareRadius, -halfSquareRadius, halfSquareRadius, halfSquareRadius);
    return result;
}