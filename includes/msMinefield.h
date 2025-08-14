#ifndef MSMINEFIELDSM_H_
#define MSMINEFIELDSM_H_

#include <stdint.h>
#include <stdio.h>

#include "msGameSM.h"

void msMinefield_terminalPrintMinefield(bool reveal);

void msMinefield_toggleFlagCell(uint8_t col, uint8_t row);

void msMinefield_revealCell(uint8_t col, uint8_t row);

bool msMinefield_isRevealed(uint8_t col, uint8_t row);

bool msMinefield_hasProx(uint8_t col, uint8_t row);

bool msMinefield_isMine(uint8_t col, uint8_t row);

void msMinefield_freeMinesweeper();

void msMinefield_updateMineProx(uint8_t col, uint8_t row);

void msMinefield_updateMinefieldProx();

void msMinefield_generateMineLocation(uint8_t safeCol, uint8_t safeRow);

void msMinefield_updateAdjTotals_Borders();

void msMinefield_initMinefield();

void msMinefield_generateMinefield();

void msMinefield_initParams(uint8_t numCols, uint8_t numRows, uint8_t num_mines);

void msMinefield_test();

#endif /* MSMINEFIELDSM_H_ */