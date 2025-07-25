#ifndef MSMINEFIELD_H_
#define MSMINEFIELD_H_

#include <stdint.h>
#include <stdio.h>

#include "msGame.h"

uint8_t initGame(Minesweeper *game, uint8_t width, uint8_t height, uint8_t num_mines);

uint8_t generateMineLocation(Minesweeper *game, uint8_t safeX, uint8_t safeY);

void placeMines(Minesweeper *game);

void updateMineProx(Minesweeper *game, uint8_t mx, uint8_t my, bool draw);

void updateMinefieldProx(Minesweeper *game);

void freeMinesweeper(Minesweeper *game);

void msMinefield_updateCell(Minesweeper *game, uint8_t rx, uint8_t ry, uint8_t type);

void printMinefield(Minesweeper *game, bool show);

void msMinefield_test();

#endif /* MSMINEFIELD_H_ */