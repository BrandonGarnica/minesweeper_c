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
    uint8_t x, y;
} Mine;

typedef struct {
    Cell **minefield;   // Pointer to 2D array of Cell structs
    Mine *mineList;     // Pointer to array of Mine struct   
    uint8_t width;      // Number of columns
    uint8_t height;     // Number of rows
    uint8_t numOfMines; // Number of rows
} Minesweeper;

extern Minesweeper msGame; // <- Exposes the global

#endif /* MSGAME_H_ */