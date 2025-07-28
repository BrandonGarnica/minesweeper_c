#ifndef MSGAME_H_
#define MSGAME_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t mineProx;
    bool isMine;
    bool isFlagged;
    bool isHidden;
} Cell;

typedef struct {
    uint8_t col, row;
} Mine;

typedef struct {
    Cell **minefield;   // Pointer to 2D array of Cell structs
    Mine *mineList;     // Pointer to array of Mine struct   
    uint8_t nCols;      // Number of columns
    uint8_t nRows;     // Number of rows
    uint8_t nMines; // Number of rows
} Minesweeper;

extern Minesweeper msGame; // <- Exposes the global

#endif /* MSGAME_H_ */