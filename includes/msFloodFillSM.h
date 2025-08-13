#ifndef MSFLOODFILLSM_H_
#define MSFLOODFILLSM_H_

#include "msGameSM.h"

#include <stdbool.h>
#include <stdint.h>

#define MAX_FF_STACK 1024

typedef enum {
    FF_SM_INIT = 0,    // Initial State
    FF_SM_IDLE,        // Wait here until interlock if off
    FF_SM_REVEAL_CURR, // Reveal current cell location
    FF_SM_CHECK_ADJ,   // Check adjcent cells
    FF_SM_DONE         // Done state
} FF_State;

typedef struct {
    // external
    Minesweeper* ffGame;
    // start col & row will be set by either msGameSM or msMinefield
    uint8_t startCol;
    uint8_t startRow;

    // internal
    FF_State ffstate;
    bool interlock;
    bool busy;
    bool done;

    // stack of cells to process
    uint8_t stackCol[MAX_FF_STACK];
    uint8_t stackRow[MAX_FF_STACK];
    uint8_t stackNextIdx[MAX_FF_STACK];
    int16_t sp; // stack size

    // current working cell
    uint8_t curCol;
    uint8_t curRow;
    uint8_t nIdx;
} FloodFillSM;

// extern FloodFillSM ffSM;

// Initializes the Flood Fill State Machine
void msFloodFillSM_init();

// Resets internal state and flags
void msFloodFillSM_reset();

// Sets the starting column and row for the flood fill
void msFloodFillSM_setStartColRow(uint8_t col, uint8_t row);

// Enables the state machine to begin flood fill (clears interlock)
void msFloodFillSM_enable();

// Disables the state machine (locks until enabled again)
void msFloodFillSM_disable();

static inline bool msFloodFillSM_ffStack_push(uint8_t col, uint8_t row, uint8_t nextIdx);

static inline bool msFloodFillSM_ffStack_pop();

// One tick of the Flood Fill State Machine
void msFloodFillSM_tick();

void msFloodFillSM_test();

#endif /* MSFLOODFILLSM_H_ */