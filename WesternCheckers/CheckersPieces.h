#pragma once

/* This file contains actions for finding and manipulating checkers pieces. */

// Gets the index of a piece. Returns -1 if the piece is not found.
int getPieceIndex(bool blue, int x, int y);

// Check how many pieces are left.
int checkRemainingPieces(bool isBlue);

// Check if it's possible to move a piece from a given location.
bool checkMovePossible(bool isBlue, int x, int y, bool isKing);

// Check if it's possible to capture a piece from a given location.
bool checkCapturePossible(bool isBlue, int x, int y, bool isKing);