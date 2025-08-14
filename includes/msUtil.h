#ifndef MSUTIL_H_
#define MSUTIL_H_

#include "msGameSM.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CELL_EMPTY                           0
#define CELL_UPDATE_PROX                     true
#define CELL_NUM_OF_SURROUNDING_CELLS        8
#define CELL_NUM_OF_SURROUNDING_CELLS_EDGE   5
#define CELL_NUM_OF_SURROUNDING_CELLS_CORNER 3

#define GAME_BEG_MF_NUM_COLS                 9
#define GAME_BEG_MF_NUM_ROWS                 GAME_BEG_MF_NUM_COLS
#define GAME_BEG_NUM_MINES                   10

#define PRINT_YES                            true
#define PRINT_NO                             !PRINT_YES

static const int8_t dx[CELL_NUM_OF_SURROUNDING_CELLS] = {0, -1, -1, -1, 0, 1, 1, 1};
static const int8_t dy[CELL_NUM_OF_SURROUNDING_CELLS] = {-1, -1, 0, 1, 1, -1, 0, 1};

bool msUtil_isInBounds(int col, int row);

#endif /* MSUTIL_H_ */