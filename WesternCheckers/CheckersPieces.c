#include <stdbool.h>
#include "CheckersConstants.h"

extern RedPieces[12][2];
extern BluePieces[12][2];

// Gets the index of a piece. Returns -1 if the piece is not found.
int getPieceIndex(bool blue, int x, int y)
{
    if (blue)
    {
        for (int i = 0; i < 12; i++)
        {
            if (BluePieces[i][0] == x && BluePieces[i][1] == y)
            {
                return i;
            }
        }
        return -1;
    }
    else
    {
        for (int i = 0; i < 12; i++)
        {
            if (RedPieces[i][0] == x && RedPieces[i][1] == y)
            {
                return i;
            }
        }
        return -1;
    }
}

// Check how many pieces are left.
int checkRemainingPieces(bool isBlue)
{
    int count = 0;
    if (isBlue)
    {
        for (int i = 0; i < 12; ++i)
        {
            if (BluePieces[i][0] != -1 && BluePieces[i][1] != -1) ++count;
        }
    }
    else // if red
    {
        for (int i = 0; i < 12; ++i)
        {
            if (RedPieces[i][0] != -1 && RedPieces[i][1] != -1) ++count;
        }
    }
    return count;
}

// Check if it's possible to capture a piece from a given location.
bool checkCapturePossible(bool isBlue, int x, int y, bool isKing)
{
    if (isBlue)
    {
        if ((getPieceIndex(false, x - 1, y - 1) >= 0 && getPieceIndex(false, x - 2, y - 2) == -1 && getPieceIndex(true, x - 2, y - 2) == -1 && onGrid(x - 2, y - 2)) || (getPieceIndex(false, x + 1, y - 1) >= 0 && getPieceIndex(false, x + 2, y - 2) == -1 && getPieceIndex(true, x + 2, y - 2) == -1) && onGrid(x + 2, y - 2))
        {
            return true;
        }
        else if (isKing && ((getPieceIndex(false, x - 1, y + 1) >= 0 && getPieceIndex(false, x - 2, y + 2) == -1 && getPieceIndex(true, x - 2, y + 2) == -1 && onGrid(x - 2, y + 2)) || (getPieceIndex(false, x + 1, y + 1) >= 0 && getPieceIndex(false, x + 2, y + 2) == -1 && getPieceIndex(true, x + 2, y + 2) == -1 && onGrid(x + 2, y + 2))))
        {
            return true;
        }
    }
    else // if red
    {
        if ((getPieceIndex(true, x + 1, y + 1) >= 0 && getPieceIndex(false, x + 2, y + 2) == -1 && getPieceIndex(true, x + 2, y + 2) == -1) && onGrid(x + 2, y + 2) || (getPieceIndex(true, x - 1, y + 1) >= 0 && getPieceIndex(false, x - 2, y + 2) == -1 && getPieceIndex(true, x - 2, y + 2) == -1) && onGrid(x - 2, y + 2))
        {
            return true;
        }
        else if (isKing && ((getPieceIndex(true, x + 1, y - 1) >= 0 && getPieceIndex(false, x + 2, y - 2) == -1 && getPieceIndex(true, x + 2, y - 2) == -1 && onGrid(x + 2, y - 2)) || (getPieceIndex(true, x - 1, y - 1) >= 0 && getPieceIndex(false, x - 2, y - 2) == -1 && getPieceIndex(true, x - 2, y - 2) == -1 && onGrid(x - 2, y - 2))))
        {
            return true;
        }
    }
    return false;
}

// Check if it's possible to move a piece from a given location.
bool checkMovePossible(bool isBlue, int x, int y, bool isKing)
{
    if ((getPieceIndex(true, x + 1, y + (isBlue ? -1 : 1)) == -1 && getPieceIndex(false, x + 1, y + (isBlue ? -1 : 1)) == -1 && onGrid(x + 1, y + (isBlue ? -1 : 1))) || (getPieceIndex(true, x - 1, y + (isBlue ? -1 : 1)) == -1 && getPieceIndex(false, x - 1, y + (isBlue ? -1 : 1)) == -1 && onGrid(x - 1, y + (isBlue ? -1 : 1))))
    {
        return true;
    }
    else if (isKing && ((getPieceIndex(true, x + 1, y + (isBlue ? 1 : -1)) == -1 && getPieceIndex(false, x + 1, y + (isBlue ? 1 : -1)) == -1 && onGrid(x + 1, y + (isBlue ? 1 : -1))) || (getPieceIndex(true, x - 1, y + (isBlue ? 1 : -1)) == -1 && getPieceIndex(false, x - 1, y + (isBlue ? 1 : -1)) == -1 && onGrid(x - 1, y + (isBlue ? 1 : -1)))))
    {
        return true;
    }
    else
    {
        return checkCapturePossible(isBlue, x, y, isKing);
    }
}
