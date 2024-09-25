#include "defs.h"
#include "game.h"
#include "move.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"

struct EnPassantInfo
{
    Square enPassantSquare;
    Piece *enPassantPawn;
    bool enPassantPossible;
};

EnPassantInfo enPassantInfo;
static Square whiteShortCastleSquare(COL_G, ROW_1);
static Square blackShortCastleSquare(COL_G, ROW_8);
static Square whiteLongCastleSquare(COL_C, ROW_1);
static Square blackLongCastleSquare(COL_C, ROW_8);

static const char *kingStr = "King";
static const char *pawnStr = "Pawn";
static const char *knightStr = "Knight";
static const char *bishopStr = "Bishop";
static const char *rookStr = "Rook";
static const char *queenStr = "Queen";
static const char *pieceStr = "pieces other than pawns or kings";

enum CastlingSide
{
    CASTLE_NULL = 0,
    CASTLE_SHORT,
    CASTLE_LONG
};

#define PIECE_NON_PAWN_OR_KING (500)

void wrongFuncErr(PieceType pieceType, int requiredPieceType, const char *funcName)
{
    const char *attemptedStr;
    const char *requiredStr;
    switch (pieceType)
    {
    case PIECE_KING:
    {
        attemptedStr = kingStr;
        break;
    }
    case PIECE_PAWN:
    {
        attemptedStr = pawnStr;
        break;
    }
    case PIECE_BISHOP:
    {
        attemptedStr = bishopStr;
        break;
    }
    case PIECE_KNIGHT:
    {
        attemptedStr = knightStr;
        break;
    }
    case PIECE_ROOK:
    {
        attemptedStr = rookStr;
        break;
    }
    case PIECE_QUEEN:
    {
        attemptedStr = queenStr;
        break;
    }
    default:
    {
        return;
    }
    }
    switch (requiredPieceType)
    {
    case PIECE_KING:
    {
        requiredStr = kingStr;
        break;
    }
    case PIECE_PAWN:
    {
        requiredStr = pawnStr;
        break;
    }
    default:
    {
        requiredStr = pieceStr;
        break;
    }
    }
    DEBUGERR("Passed a %s for function %s, which should only be used for a %s\n", attemptedStr, funcName, requiredStr);
}

Square getShortCastleSquare()
{
    Square result = player->side == SIDE_WHITE ? whiteShortCastleSquare : blackShortCastleSquare;
    return result;
}

Square getLongCastleSquare()
{
    Square result = player->side == SIDE_WHITE ? whiteLongCastleSquare : blackLongCastleSquare;
    return result;
}

bool isEnPassantSquare(Square square)
{
    if (!enPassantInfo.enPassantPossible)
    {
        return false;
    }
    bool result = enPassantInfo.enPassantSquare == square;
    return result;
}

bool isEnPassantSquare(int column, int row)
{
    bool result = isEnPassantSquare(Square(column, row));
    return result;
}

void setEnPassantForPawn(Piece *piece)
{
    if (!piece || piece->type != PIECE_PAWN)
    {
        enPassantInfo.enPassantPawn = NULL;
        enPassantInfo.enPassantSquare = getNullSquare();
        enPassantInfo.enPassantPossible = false;
        return;
    }
    enPassantInfo.enPassantPawn = piece;
    enPassantInfo.enPassantSquare = Square(piece->square.column, piece->square.row - player->side.pawnAdvanceDirection);
    enPassantInfo.enPassantPossible = true;
}

void addSquare(Square square, ValueBoard *board)
{
    board->increaseValue(square);
}

void addSquare(int column, int row, ValueBoard *board)
{
    addSquare(Square(column, row), board);
}

void tryAddSquare(int column, int row, ValueBoard *board)
{
    if (isValidSquare(column, row))
    {
        addSquare(column, row, board);
    }
}

/*
Only for pieces, not pawns.
includeBlockingPiece determines the visibility of a square that is occupied by a piece or a pawn of the same color.
*/
void calculateVisibleSquaresInDirection(Square square, int columnDir, int rowDir, Side side, ValueBoard *board, bool includeBlockingPiece)
{
    int terminalColumn = columnDir == -1 ? COL_A : (columnDir == 0 ? NULL_COLUMN : NUM_COLUMNS);
    int terminalRow = rowDir == -1 ? ROW_1 : (rowDir == 0 ? NULL_ROW : NUM_ROWS);
    Square currentSquare(square);

    while (currentSquare.column != terminalColumn && currentSquare.row != terminalRow)
    {
        currentSquare.column += columnDir;
        currentSquare.row += rowDir;
        if (!includeBlockingPiece && isOccupiedSquareBy(currentSquare, side))
        {
            break;
        }
        // add square if it is occupied by opposing colored piece, and finish calculating.
        addSquare(currentSquare, board);
        if (isOccupiedSquareBy(currentSquare, !side))
        {
            return;
        }
    }
}

/*
Not for pawns and kings, as their rules are different from other pieces.
includeBlockingPiece determines the visibility of a square that is occupied by a piece or a pawn of the same color.
*/
void calculateVisibleSquaresForPieceType(Square square, PieceType type, Side side, ValueBoard *board, bool includeBlockingPiece)
{
    switch (type)
    {
    case PIECE_PAWN:
    case PIECE_KING:
    {
        DEBUGCALL(wrongFuncErr(type, PIECE_NON_PAWN_OR_KING, "calculateVisibleSquaresForPieceType"));
        return;
    }
    case PIECE_BISHOP:
    {
        calculateVisibleSquaresInDirection(square, -1, 1, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, -1, -1, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, 1, -1, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, 1, 1, side, board, includeBlockingPiece);
        return;
    }
    case PIECE_KNIGHT:
    {
        int column = square.column;
        int row = square.row;
        Square possibleSquares[8]{{column - 1, row + 2},
                                  {column + 1, row + 2},
                                  {column + 2, row + 1},
                                  {column + 2, row - 1},
                                  {column + 1, row - 2},
                                  {column - 1, row - 2},
                                  {column - 2, row + 1},
                                  {column - 2, row - 1}};
        for (int i = 0; i < 8; ++i)
        {
            Square possibleSquare = possibleSquares[i];
            if (isValidSquare(possibleSquare))
            {
                if (!includeBlockingPiece && isOccupiedSquareBy(possibleSquare, side))
                {
                    continue;
                }
                addSquare(possibleSquare, board);
            }
        }
        return;
    }
    case PIECE_ROOK:
    {
        calculateVisibleSquaresInDirection(square, -1, 0, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, 0, 1, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, 0, -1, side, board, includeBlockingPiece);
        calculateVisibleSquaresInDirection(square, 1, 0, side, board, includeBlockingPiece);
        return;
    }
    case PIECE_QUEEN:
    {
        calculateVisibleSquaresForPieceType(square, PIECE_BISHOP, side, board, includeBlockingPiece);
        calculateVisibleSquaresForPieceType(square, PIECE_ROOK, side, board, includeBlockingPiece);
        return;
    }
    }
}

int getPawnAdvanceDirection(Side side)
{
    int result = side == SIDE_WHITE ? 1 : -1;
    return result;
}

/*
Not for pawns and kings, as their rules are different from other pieces.
includeBlockingPiece determines the visibility of a square that is occupied by a piece or a pawn of the same color.
*/
void calculateVisibleSquaresForPiece(Piece *piece, ValueBoard *board, bool includeBlockingPiece)
{
    Side side = piece->side;
    calculateVisibleSquaresForPieceType(piece->square, piece->type, piece->side, board, includeBlockingPiece);
}

void calculateControlSquaresForPawn(Piece *piece, ValueBoard *board)
{
    if (piece->type != PIECE_PAWN)
    {
        DEBUGCALL(wrongFuncErr(piece->type, PIECE_PAWN, "calculateControlSquaresForPawn"));
        return;
    }
    int controlDir = getPawnAdvanceDirection(piece->side);
    Square square = piece->square;
    if (isValidSquare(square.column - 1, square.row + controlDir))
    {
        addSquare(square.column - 1, square.row + controlDir, board);
    }
    if (isValidSquare(square.column + 1, square.row + controlDir))
    {
        addSquare(square.column + 1, square.row + controlDir, board);
    }
}

void calculateMovementSquaresForPawn(Piece *piece)
{
    if (piece->type != PIECE_PAWN)
    {
        DEBUGCALL(wrongFuncErr(piece->type, PIECE_PAWN, "calculateMovementSquaresForPawn"));
        return;
    }
    int movementDir = player->side.pawnAdvanceDirection;
    int numPossibleAdvancements = isPawnStartingRow(piece->square.row, piece->side) ? 2 : 1;
    Square square = piece->square;
    for (int i = 1; i <= numPossibleAdvancements; ++i)
    {
        if (isOccupiedSquare(square.column, square.row + i * movementDir))
        {
            break;
        }
        addSquare(square.column, square.row + i * movementDir, &piece->movementSquares);
    }
    if (isOccupiedSquareBy(square.column - 1, square.row + movementDir, !piece->side) || isEnPassantSquare(square.column - 1, square.row + movementDir))
    {
        addSquare(square.column - 1, square.row + movementDir, &piece->movementSquares);
    }
    if (isOccupiedSquareBy(square.column + 1, square.row + movementDir, !piece->side) || isEnPassantSquare(square.column + 1, square.row + movementDir))
    {
        addSquare(square.column + 1, square.row + movementDir, &piece->movementSquares);
    }
}

CastlingSide isCastlingSquare(Square square)
{
    CastlingSide result = CASTLE_NULL;

    bool shortCastleSide = square == getShortCastleSquare();
    if (shortCastleSide)
    {
        result = CASTLE_SHORT;
    }
    else
    {
        bool longCastleSide = square == getLongCastleSquare();
        if (longCastleSide)
        {
            result = CASTLE_LONG;
        }
    }
    return result;
}

bool isCastlingRights()
{
    bool result = player->shortCastleRights || player->longCastleRights;
    return result;
}

bool isEnemyControlledSquare(int column, int row)
{
    bool result = players[!turn].controlSquares.getSquareValue(column, row) > 0;
    return result;
}

bool isEnemyControlledSquare(Square square)
{
    bool result = isEnemyControlledSquare(square.column, square.row);
    return result;
}

bool isKingTraversableSquare(int column, int row, bool includeOnlyEmptySquares)
{
    if ((includeOnlyEmptySquares && isOccupiedSquare(column, row)) || isOccupiedSquareBy(column, row, player->side.side))
    {
        return false;
    }
    if (isEnemyControlledSquare(column, row))
    {
        return false;
    }
    return true;
}

bool isRowEmpty(int row, int columnStart, int columnEndInclusive)
{
    FOR_IN_I(col, columnStart, columnEndInclusive)
    {
        if (isOccupiedSquare(col, row))
        {
            return false;
        }
    }
    return true;
}

bool isShortCastleLegal()
{
    if (!player->shortCastleRights)
    {
        return false;
    }
    int row = player->side.pieceRow;
    FOR_IN_I(col, COL_F, COL_G)
    {
        if (!isKingTraversableSquare(col, row, true))
        {
            return false;
        }
    }
    return true;
}

bool isLongCastleLegal()
{
    if (!player->longCastleRights)
    {
        return false;
    }
    int row = player->side.pieceRow;
    FOR_IN_I(col, COL_B, COL_D)
    {
        if (!isKingTraversableSquare(col, row, true))
        {
            return false;
        }
    }
    return true;
}

bool kingIsInCheck()
{
    bool result = isEnemyControlledSquare(player->king->square);
    return result;
}

bool isLegalCastlingMove(Piece *piece, Square square)
{
    if (piece->type != PIECE_KING || !isCastlingRights() || !isCastlingSquare(square))
    {
        return false;
    }
    bool shortCastle = square.column == COL_G;
    bool result;
    if (shortCastle)
    {
        result = isShortCastleLegal();
    }
    else
    {
        result = isLongCastleLegal();
    }
    return result;
}

void calculateVisibleSquaresForKing(Piece *piece, ValueBoard *board, bool includeNonTraversableSquares)
{
    if (piece->type != PIECE_KING)
    {
        DEBUGCALL(wrongFuncErr(piece->type, PIECE_KING, "calculateVisibleSquaresForKing"));
        return;
    }
    int column = piece->square.column;
    int row = piece->square.row;
    Square possibleSquares[8]{{column - 1, row + 1},
                              {column - 1, row + 0},
                              {column - 1, row - 1},
                              {column + 0, row + 1},
                              {column + 0, row - 1},
                              {column + 1, row + 1},
                              {column + 1, row - 0},
                              {column + 1, row - 1}};
    for (int i = 0; i < 8; ++i)
    {
        Square possibleSquare = possibleSquares[i];
        int currentColumn = possibleSquare.column;
        int currentRow = possibleSquare.row;
        if (isValidSquare(currentColumn, currentRow))
        {
            if (!includeNonTraversableSquares && !isKingTraversableSquare(currentColumn, currentRow, false))
            {
                continue;
            }
            addSquare(currentColumn, currentRow, board);
        }
    }
}

void calculateControlSquaresForKing(Piece *piece, ValueBoard *board)
{
    calculateVisibleSquaresForKing(piece, board, true);
}

void calculateMovementSquaresForKing(Piece *piece)
{
    calculateVisibleSquaresForKing(piece, &piece->movementSquares, false);
    if (isShortCastleLegal())
    {
        addSquare(getShortCastleSquare(), &piece->movementSquares);
    }
    if (isLongCastleLegal())
    {
        addSquare(getLongCastleSquare(), &piece->movementSquares);
    }
}

void calculateControlSquaresForPiece(Piece *piece)
{
    if (!piece)
    {
        return;
    }
    ValueBoard *board = piece->side == SIDE_WHITE ? &players[SIDE_WHITE].controlSquares : &players[SIDE_BLACK].controlSquares;
    switch (piece->type)
    {
    case PIECE_PAWN:
    {
        calculateControlSquaresForPawn(piece, board);
        break;
    }
    case PIECE_KING:
    {
        calculateControlSquaresForKing(piece, board);
        break;
    }
    default:
    {
        calculateVisibleSquaresForPiece(piece, board, true);
    }
    }
}

void calculateMovementSquaresForPiece(Piece *piece)
{
    if (!piece)
    {
        return;
    }
    piece->movementSquares.clear();
    if (kingIsInCheck() && piece->type != PIECE_KING) 
    {
        return;
    }
    switch (piece->type)
    {
    case PIECE_KING:
    {
        calculateMovementSquaresForKing(piece);
        break;
    }
    case PIECE_PAWN:
    {
        calculateMovementSquaresForPawn(piece);
        break;
    }
    default:
    {
        calculateVisibleSquaresForPiece(piece, &piece->movementSquares, false);
        break;
    }
    }
}

bool capture(Piece *piece)
{
    if (!piece)
    {
        return false;
    }
#ifdef DEBUG
    if (piece->side == turn)
    {
        DEBUGERR("tried to capture own piece!\n");
        return false;
    }
#endif
    killPiece(piece);
    return true;
}

bool captureIfOccupied(Square square)
{
    Piece *piece = getPieceFromSquare(square);
    bool result = capture(piece);
    return result;
}

bool isDoublePawnAdvancement(const Piece *piece, int nextRow)
{
    bool result = (piece->type == PIECE_PAWN && jadel::absInt(nextRow - piece->square.row) == 2);
    return result;
}

void setPieceOnSquare(Piece *piece, Square square)
{
    captureIfOccupied(square);
    piece->square = square;
}

void setCastlingLongImpossible()
{
    player->longCastleRights = false;
}

void setCastlingShortImpossible()
{
    player->shortCastleRights = false;
}

void setCastlingImpossible()
{
    setCastlingShortImpossible();
    setCastlingLongImpossible();
}

void moveKing(Piece *piece, Square square)
{
    CastlingSide castlingSide = isCastlingSquare(square);
    if (castlingSide == CASTLE_NULL || !isCastlingRights())
    {
        setPieceOnSquare(piece, square);
        return;
    }
    CastlingInfo info = castlingSide == CASTLE_SHORT ? player->side.shortCastlingInfo : player->side.longCastlingInfo;

    Piece *rook = getPieceFromSquare(info.rookStartSquare);
    setPieceOnSquare(rook, info.rookEndSquare);
    setPieceOnSquare(piece, square);
    setCastlingImpossible();
    setEnPassantForPawn(NULL);
}

void moveRook(Piece *piece, Square square)
{
    if (piece->square == player->side.shortCastlingInfo.rookStartSquare)
    {
        setCastlingShortImpossible();
    }
    else
    {
        setCastlingLongImpossible();
    }
    setPieceOnSquare(piece, square);
    setEnPassantForPawn(NULL);
}

void captureEnPassant()
{
    capture(enPassantInfo.enPassantPawn);
    setEnPassantForPawn(NULL);
}

void movePawn(Piece *piece, Square square)
{
    
    if (enPassantInfo.enPassantPossible && square == enPassantInfo.enPassantSquare)
    {
        captureEnPassant();
        setPieceOnSquare(piece, square);
    }
    else if (isDoublePawnAdvancement(piece, square.row))
    {
        setPieceOnSquare(piece, square);
        setEnPassantForPawn(piece);
        return;
    }
    else
    {
        if (square.row == players[!turn].side.pieceRow)
        {
            submitForPromotion(piece);
        }
        setPieceOnSquare(piece, square);
        setEnPassantForPawn(NULL);
    }
    
}

void move(Piece *piece, Square square)
{
    switch (piece->type)
    {
    case PIECE_PAWN:
    {
        movePawn(piece, square);
        break;
    }
    case PIECE_KING:
    {
        moveKing(piece, square);
        break;
    }
    case PIECE_ROOK:
    {
        moveRook(piece, square);
        break;
    }
    default:
    {
        setPieceOnSquare(piece, square);
        setEnPassantForPawn(NULL);
    }
    }
}

bool tryToMove(Piece *piece, Square square)
{
    if ((piece->side != turn) || (piece->square == square))
    {
        return false;
    }
    if (!isValidSquare(square))
    {
        DEBUGERR("Tried to move piece to an invalid square: column %d, row %d\n", square);
        return false;
    }
    int squareValue = piece->movementSquares.getSquareValue(square);
    if (squareValue > 0)
    {
        move(piece, square);
        return true;
    }
#ifdef DEBUG
    if (squareValue > 1)
    {
        DEBUGERR("Piece movement square value is %d. Should never be higher that 1!\n", squareValue);
    }
#endif
    return false;
}