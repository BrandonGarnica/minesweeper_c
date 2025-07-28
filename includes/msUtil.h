#ifndef MSUTIL_H_
#define MSUTIL_H_

#include "msGame.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define CELL_EMPTY 0
#define CELL_UPDATE_PROX true
#define CELL_NUM_OF_SURROUNDING_CELLS 8

#define GAME_BEG_MF_NUM_COLS 9
#define GAME_BEG_MF_NUM_ROWS GAME_BEG_MF_NUM_COLS
#define GAME_BEG_NUM_MINES 10

#define PRINT_YES true
#define PRINT_NO !PRINT_YES

bool msUtil_isInBounds(Minesweeper *game, int col, int row);

#endif /* MSUTIL_H_ */