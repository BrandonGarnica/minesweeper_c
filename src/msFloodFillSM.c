#include "msFloodFillSM.h"
#include "msGameSM.h"
#include "msMinefield.h"
#include "msUtil.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_UINT8_T                    255
#define NUM_OF_ADJ_MINES               8

#define LOCKED                         true
#define UNLOCKED                       false

#define ERROR_LOACTION_FF_1            "[FF_SM]:1 "
#define ERROR_LOACTION_FF_2            "[FF_SM]:2 "
#define ERROR_STATEMENT_STACK_OVERFLOW "Stack overflow prevented\n"

FloodFillSM ffSM;

static bool interlock = LOCKED;

// Init SM
void msFloodFillSM_init() {
    // Copy msGame to ffSM game struct
    ffSM.ffGame = &msGame;
    // Init SM
    ffSM.ffstate = FF_SM_INIT;
}

void msFloodFillSM_reset() {
    ffSM.ffstate = FF_SM_INIT;
    interlock = LOCKED;
}

void msFloodFillSM_setStartColRow(uint8_t col, uint8_t row) {
    ffSM.startCol = col;
    ffSM.startRow = row;
}

void msFloodFillSM_enable() { interlock = UNLOCKED; }

void msFloodFillSM_disable() { interlock = LOCKED; }

// Steps for FF

// Assumptions:
// start cell is NOT A MINE
// Start cell DOES NOT HAVE A PROX
// Start cell is ALWAYS A ZERO

// 1) Reveal current cell
// Reveal current cell -> Check Adj Cells

// 2) Check Adj Cells
// If cell is out of bounds -> Process Next Cell
// If cell has already been revealed -> Process Next Cell
// Reveal Cell
// If cell has prox, don't include in stack
// If cell has 0, stash location in stack - Check Adj Cells
// Iterate 8 times (Number of adj cells)

// 3) Process Cell
// Pop cell location off stack.
// Reveal next cell in stack -> Reveal Current Cell.
// When stack is empty -> Done

// 4) Done

static inline bool msFloodFillSM_ffStack_push(uint8_t col, uint8_t row) {
    if (ffSM.sp >= MAX_FF_STACK) {
        return false;
    }
    ffSM.stackCol[ffSM.sp] = col;
    ffSM.stackRow[ffSM.sp] = row;
    ffSM.sp++;
    return true;
}

static inline bool msFloodFillSM_ffStack_pop(uint8_t* col, uint8_t* row) {
    if (ffSM.sp == 0) {
        return false;
    }
    // Dec stack position
    ffSM.sp--;
    // Pop top
    *col = ffSM.stackCol[ffSM.sp];
    *row = ffSM.stackRow[ffSM.sp];
    return true;
}

void msFloodFillSM_tick() {

    int8_t dxCol;
    int8_t dxRow;

    // Transition Actions

    switch (ffSM.ffstate) {
        case FF_SM_INIT: ffSM.ffstate = FF_SM_IDLE; break;

        case FF_SM_IDLE:

            // Stay in idle state until interlock is unlocked
            if (!interlock) {
                ffSM.curCol = ffSM.startCol;
                ffSM.curRow = ffSM.startRow;
                ffSM.nIdx = 0;
                ffSM.sp = 0; // ensure clean stack
                ffSM.busy = true;
                ffSM.done = false;
                ffSM.ffstate = FF_SM_REVEAL_CURR;
            }
            break;

        case FF_SM_REVEAL_CURR:

            Cell* curCell = &msGame.minefield[ffSM.curCol][ffSM.curRow];

            if (msMinefield_isMine(curCell) || msMinefield_hasProx(curCell)) {
                ffSM.ffstate = FF_SM_DONE;
            } else {
                ffSM.ffstate = FF_SM_CHECK_ADJ;
            }

            break;

        case FF_SM_CHECK_ADJ:

            // Stay in this loop 8 times to check all adj cells
            if (ffSM.nIdx >= NUM_OF_ADJ_MINES) {
                // printf("SM: FF_SM_CHECK_ADJ -> FF_SM_PROCESS_CELLS\n");
                // Reset nIdx
                ffSM.nIdx = 0;
                ffSM.ffstate = FF_SM_PROCESS_CELLS;
                break;
            }

            break;

        case FF_SM_PROCESS_CELLS:
            // ffSM.ffstate = FF_SM_REVEAL_CURR;
            ffSM.ffstate = FF_SM_CHECK_ADJ;
            // printf("Stack Position: %d\n", ffSM.sp);
            if (ffSM.sp == 0) {
                ffSM.ffstate = FF_SM_DONE;
            }

            break;

        case FF_SM_DONE: break;

        default: break;
    }

    // State Actions

    switch (ffSM.ffstate) {
        case FF_SM_INIT:
            dxCol = 0;
            dxRow = 0;
            ffSM.startCol = 0;
            ffSM.startRow = 0;
            ffSM.curCol = 0;
            ffSM.curRow = 0;
            ffSM.nIdx = 0;
            ffSM.sp = 0;
            ffSM.busy = false;
            ffSM.done = false;
            break;

        case FF_SM_IDLE: break;

        case FF_SM_REVEAL_CURR:
            // Assumption in reveal state:
            // Current cell HAS NOT BEEN REVEALED
            // Current cell is in game BOUNDS
            msMinefield_revealCell(ffSM.curCol, ffSM.curRow);
            break;

        case FF_SM_CHECK_ADJ:
            dxCol = ffSM.curCol + dx[ffSM.nIdx];
            dxRow = ffSM.curRow + dy[ffSM.nIdx];
            ffSM.nIdx++;
            // printf("\tCuCol: %d, CuRow: %d\n", ffSM.curCol + 1, ffSM.curRow + 1);
            // printf("\tdxCol: %d, dxRow: %d\n", dxCol + 1, dxRow + 1);
            // printf("\tCell Index: %d\n", ffSM.nIdx);
            // Check if dxCol and dxRow are in bounds
            if (!msUtil_isInBounds(dxCol, dxRow)) {
                // printf("\tCell at (Col: %d, row: %d) IS NOT IN BOUNDS.\n", dxCol + 1, dxRow + 1);
                break;
            }

            Cell* dxCell = &msGame.minefield[dxCol][dxRow];

            // Check is dxCell has already been revealed
            if (msMinefield_isRevealed(dxCell)) {
                // printf("\tCell at (Col: %d, row: %d) already revealed.\n", dxCol + 1, dxRow + 1);
                break;
            }

            msMinefield_revealCell(dxCol, dxRow);

            // Store cells that are zero in stack
            if (dxCell->mineProx == 0) {
                msFloodFillSM_ffStack_push(dxCol, dxRow);
                // printf("\tCell at (Col: %d, row: %d) is a zero, saved to stack.\n", dxCol + 1, dxRow + 1);
                // printf("\tStack Position: %d\n", ffSM.sp);
            }
            break;

        case FF_SM_PROCESS_CELLS:
            // Set new current cell to top of cell stack
            if (msFloodFillSM_ffStack_pop(&ffSM.curCol, &ffSM.curRow)) {
                // printf("\tPopping stack, current Col: %d & Row: %d\n", ffSM.curCol + 1, ffSM.curRow + 1);
                // printf("\tStack Position: %d\n", ffSM.sp);
                break;
            }
        case FF_SM_DONE:

            ffSM.busy = false;
            ffSM.done = true;
            interlock = LOCKED;
            break;

        default: break;
    }
}

void msFloodFillSM_test() {

    printf("---| msFloodFill.c Test |---\n");

    printf("Beginner Game: \n");

    msMinefield_initGame(GAME_BEG_MF_NUM_COLS, GAME_BEG_MF_NUM_ROWS, GAME_BEG_NUM_MINES);
    msMinefield_generateMineLocation(4, 4);
    msMinefield_updateMinefieldProx();

    msMinefield_terminalPrintMinefield(PRINT_YES);

    // Init and run SM
    msFloodFillSM_reset();
    msFloodFillSM_init();
    msFloodFillSM_setStartColRow(4, 4);
    msFloodFillSM_enable();

    // Run until done or safety cap
    uint16_t nTicks = 1;
    while (!ffSM.done) {
        msFloodFillSM_tick();
        nTicks++;
        // msMinefield_terminalPrintMinefield(PRINT_NO);
    }

    // Check results
    printf("FloodFill finished in %d ticks\n", nTicks);

    msMinefield_terminalPrintMinefield(PRINT_NO);
}