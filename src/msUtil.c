#include "msGameSM.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// msUtil.c
bool msUtil_isInBounds(Minesweeper* game, int col, int row) {
    return (col >= 0 && col < game->nCols && row >= 0 && row < game->nRows);
}