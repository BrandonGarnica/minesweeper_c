#ifndef MSMINEFIELDSM_H_
#define MSMINEFIELDSM_H_

#include <stdint.h>
#include <stdio.h>

#include "msGameSM.h"

uint8_t msMinefield_initGame(Minesweeper *game, uint8_t numCols, uint8_t numRows, uint8_t num_mines);

uint8_t msMinefield_generateMineLocation(Minesweeper *game, uint8_t safeCol, uint8_t safeRow);

void msMinefield_updateMineProx(Minesweeper *game, uint8_t col, uint8_t row);

void msMinefield_updateMinefieldProx(Minesweeper *game);

void msMinefield_toggleFlagCell(Minesweeper *game, uint8_t col, uint8_t row);

void msMinefield_revealCell(Minesweeper *game, uint8_t col, uint8_t row);

void msMinefield_freeMinesweeper(Minesweeper *game);

void msMinefield_consolePrintMinefield(Minesweeper *game, bool show);

void msMinefield_test();

#endif /* MSMINEFIELDSM_H_ */