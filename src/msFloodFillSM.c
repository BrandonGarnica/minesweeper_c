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

int8_t dxCol;
int8_t dxRow;
int8_t dxIdx;

// Push current frame with resume index
static inline bool msFloodFillSM_ffStack_push(uint8_t col, uint8_t row, uint8_t nextIdx) {
    if (ffSM.sp >= MAX_FF_STACK) return false;
    ffSM.stackCol[ffSM.sp] = col;
    ffSM.stackRow[ffSM.sp] = row;
    ffSM.stackNextIdx[ffSM.sp] = nextIdx; // resume neighbor slot after returning
    ffSM.sp++;
    return true;
}

// Pop frame and restore current and neighbor index
static inline bool msFloodFillSM_ffStack_pop(void) {
    if (ffSM.sp == 0) return false;
    ffSM.sp--;
    ffSM.curCol = ffSM.stackCol[ffSM.sp];
    ffSM.curRow = ffSM.stackRow[ffSM.sp];
    ffSM.nIdx = ffSM.stackNextIdx[ffSM.sp];
    return true;
}

// Init SM
void msFloodFillSM_init(void) {
    ffSM.busy = false;
    ffSM.done = false;
    ffSM.ffGame = &msGame; // mirror msGame pointer if you need it elsewhere
    ffSM.ffstate = FF_SM_INIT;
}

void msFloodFillSM_reset(void) {
    ffSM.busy = false;
    ffSM.done = false;
    ffSM.ffstate = FF_SM_INIT;
    ffSM.interlock = LOCKED;
}

void msFloodFillSM_setStartColRow(uint8_t col, uint8_t row) {
    ffSM.startCol = col;
    ffSM.startRow = row;
}

void msFloodFillSM_enable(void) { ffSM.interlock = UNLOCKED; }

void msFloodFillSM_disable(void) { ffSM.interlock = LOCKED; }

void msFloodFillSM_tick(void) {

    // TRANSITIONS
    switch (ffSM.ffstate) {
        case FF_SM_INIT: ffSM.ffstate = FF_SM_IDLE; break;

        case FF_SM_IDLE:
            if (!ffSM.interlock) {
                ffSM.curCol = ffSM.startCol;
                ffSM.curRow = ffSM.startRow;
                ffSM.nIdx = 0;
                ffSM.sp = 0;
                ffSM.busy = true;
                ffSM.done = false;
                ffSM.ffstate = FF_SM_REVEAL_CURR;
            }
            break;

        case FF_SM_REVEAL_CURR:
            // This stat is for us to decide in the futur if we want FF to handle game logic
            // or to have a differen SM handle that logic.
            Cell* revCell = &msGame.minefield[ffSM.curCol][ffSM.curRow];
            msMinefield_revealCell(ffSM.curCol, ffSM.curRow);

            // If the start is a number or a mine, we are done. Otherwise descend into neighbors.
            if (msMinefield_isMine(revCell) || msMinefield_hasProx(revCell)) {
                ffSM.ffstate = FF_SM_DONE;
                // Raise some kind of flag here for other SM logic
            } else {
                ffSM.ffstate = FF_SM_CHECK_ADJ;
            }
            break;

        case FF_SM_CHECK_ADJ:
            // Use O(1) early-out based on counters
            Cell* curCell = &msGame.minefield[ffSM.curCol][ffSM.curRow];
            bool exhausted = (curCell->adjRevealed == curCell->adjTotal);

            if (ffSM.nIdx >= NUM_OF_ADJ_MINES || (ffSM.nIdx == 0 && exhausted)) {
                if (!msFloodFillSM_ffStack_pop()) ffSM.ffstate = FF_SM_DONE;
            }
            break;
    }

    // STATE ACTIONS
    switch (ffSM.ffstate) {
        case FF_SM_INIT:
            ffSM.startCol = 0;
            ffSM.startRow = 0;
            ffSM.curCol = 0;
            ffSM.curRow = 0;
            ffSM.nIdx = 0;
            ffSM.sp = 0;
            break;

        case FF_SM_IDLE: break;

        case FF_SM_REVEAL_CURR: break;

        case FF_SM_CHECK_ADJ:
            dxCol = ffSM.curCol + dx[ffSM.nIdx];
            dxRow = ffSM.curRow + dy[ffSM.nIdx];
            dxIdx = ++ffSM.nIdx; // pre-increment so parent resumes at the next slot

            // Skip out-of-bounds
            if (!msUtil_isInBounds(dxCol, dxRow)) break;

            Cell* dxCell = &msGame.minefield[dxCol][dxRow];

            // Skip already revealed
            if (msMinefield_isRevealed(dxCell)) break;

            // Reveal neighbor
            msMinefield_revealCell(dxCol, dxRow);

            // If neighbor is zero, descend immediately: push current frame and switch focus
            if (dxCell->mineProx == 0) {
                if (msFloodFillSM_ffStack_push(ffSM.curCol, ffSM.curRow, dxIdx)) {
                    ffSM.curCol = dxCol;
                    ffSM.curRow = dxRow;
                    ffSM.nIdx = 0;
                }
            }
            break;

        case FF_SM_DONE:
            ffSM.busy = false;
            ffSM.done = true;
            msFloodFillSM_disable();
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
    }

    // Check results
    printf("FloodFill finished in %d ticks\n", nTicks);
    nTicks = 1;

    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();

    printf("Intermediate Game: \n");

    msMinefield_initGame(16, 16, 40);
    msMinefield_generateMineLocation(7, 7);
    msMinefield_updateMinefieldProx();

    msMinefield_terminalPrintMinefield(PRINT_YES);

    // Init and run SM
    msFloodFillSM_reset();
    msFloodFillSM_init();
    msFloodFillSM_setStartColRow(7, 7);
    msFloodFillSM_enable();

    while (!ffSM.done) {
        msFloodFillSM_tick();
        nTicks++;
    }

    // Check results
    printf("FloodFill finished in %d ticks\n", nTicks);
    nTicks = 0;

    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();

    printf("Expert Game: \n");

    msMinefield_initGame(30, 16, 99);
    msMinefield_generateMineLocation(14, 9);
    msMinefield_updateMinefieldProx();

    msMinefield_terminalPrintMinefield(PRINT_YES);

    // Init and run SM
    msFloodFillSM_reset();
    msFloodFillSM_init();
    msFloodFillSM_setStartColRow(14, 9);
    msFloodFillSM_enable();

    while (!ffSM.done) {
        msFloodFillSM_tick();
        nTicks++;
    }

    // Check results
    printf("FloodFill finished in %d ticks\n", nTicks);
    nTicks = 0;

    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();
}