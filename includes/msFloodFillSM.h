#ifndef MSFLOODFILLSM_H_
#define MSFLOODFILLSM_H_

#include "msGameSM.h"

#include <stdint.h>

void msFloodFill_FF(Minesweeper *game, uint8_t row, uint8_t col);

void msFloodFill_test();

#endif /* MSFLOODFILLSM_H_ */