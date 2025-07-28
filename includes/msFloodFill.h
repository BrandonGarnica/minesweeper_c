#ifndef MSFLOODFILL_H_
#define MSFLOODFILL_H_
#include <stdint.h>
#include "msGame.h"

void msFloodFill_FF(Minesweeper *game, uint8_t row, uint8_t col);

void msFloodFill_test();

#endif /* MSFLOODFILL_H_ */