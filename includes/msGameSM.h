#ifndef MSGAMESM_H_
#define MSGAMESM_H_

#include <stdbool.h>
#include <stdint.h>

#include "msUtil.h"

typedef struct {
    bool isMine;
    bool isFlagged;
    bool isHidden;
    uint8_t mineProx;    // 0..8
    uint8_t adjTotal;    // neighbors in-bounds: 3,5,8,...
    uint8_t adjRevealed; // neighbors currently revealed: 0..adjTotal
} Cell;

typedef struct {
    uint8_t col, row;
} Mine;

typedef struct {
    Cell** minefield; // Pointer to 2D array of Cell structs
    Mine* mineList;   // Pointer to array of Mine struct
    uint8_t nCols;    // Number of columns
    uint8_t nRows;    // Number of rows
    uint8_t nMines;   // Number of rows
} Minesweeper;

extern Minesweeper msGame; // <- Exposes the global

#endif /* MSGAMESM_H_ */