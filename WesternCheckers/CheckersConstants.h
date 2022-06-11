#pragma once

/* This file contains macros and constants for the American Checkers game. */

#define boardToWindowX(coord) ((double)(coord + 1) / 2.0 + 0.5)
#define boardToWindowY(coord) ((double)(coord + 1) / 2.0 + 0.5)
#define windowToBoardX(coord) ((int)(ScaleXInches(x) * 2) - 2)
#define windowToBoardY(coord) ((int)(ScaleYInches(y) * 2) - 2)
#define onGrid(x, y) (0 <= x && x < 8 && 0 <= y && y < 8)
#define RED_START { {0, 0}, {0, 2}, {1, 1}, {2, 0}, {2, 2}, {3, 1}, {4, 0}, {4, 2}, {5, 1}, {6, 0}, {6, 2}, {7, 1} }
#define BLUE_START { {0, 6}, {1, 5}, {1, 7}, {2, 6}, {3, 5}, {3, 7}, {4, 6}, {5, 5}, {5, 7}, {6, 6}, {7, 5}, {7, 7} }

// Time limit is 5 minutes, which is the official time limit.
#define TIME_LIMIT 300