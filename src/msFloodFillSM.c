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

int8_t dxCol;
int8_t dxRow;
int8_t dxIdx;

// Push current frame with resume index
static inline bool msFloodFillSM_ffStack_push(uint8_t col, uint8_t row, uint8_t nextIdx) {
    if (ffSM.sp >= MAX_FF_STACK) return false;
    ffSM.stackCol[ffSM.sp]     = col;
    ffSM.stackRow[ffSM.sp]     = row;
    ffSM.stackNextIdx[ffSM.sp] = nextIdx;   // resume neighbor slot after returning
    ffSM.sp++;
    return true;
}

// Pop frame and restore current and neighbor index
static inline bool msFloodFillSM_ffStack_pop(void) {
    if (ffSM.sp == 0) return false;
    ffSM.sp--;
    ffSM.curCol = ffSM.stackCol[ffSM.sp];
    ffSM.curRow = ffSM.stackRow[ffSM.sp];
    ffSM.nIdx   = ffSM.stackNextIdx[ffSM.sp];
    return true;
}

// Returns true if every in‑bounds neighbor is already revealed
static inline bool msFloodFillSM_allAdjRevealed(uint8_t col, uint8_t row) {
    for (uint8_t i = 0; i < 8; i++) {
        int8_t ncol = (int8_t)col + dx[i];
        int8_t nrow = (int8_t)row + dy[i];
        if (!msUtil_isInBounds(ncol, nrow)) continue;
        Cell* nb = &msGame.minefield[(uint8_t)ncol][(uint8_t)nrow];
        if (!msMinefield_isRevealed(nb)) return false;
    }
    return true;
}


// Init SM
void msFloodFillSM_init(void) {
    ffSM.ffGame  = &msGame;    // mirror msGame pointer if you need it elsewhere
    ffSM.ffstate = FF_SM_INIT;
}

void msFloodFillSM_reset(void) {
    ffSM.ffstate = FF_SM_INIT;
    interlock    = LOCKED;
}

void msFloodFillSM_setStartColRow(uint8_t col, uint8_t row) {
    ffSM.startCol = col;
    ffSM.startRow = row;
}

void msFloodFillSM_enable(void)  { interlock = UNLOCKED; }
void msFloodFillSM_disable(void) { interlock = LOCKED; }

void msFloodFillSM_tick(void) {

    // TRANSITIONS
    switch (ffSM.ffstate) {
        case FF_SM_INIT:
            ffSM.ffstate = FF_SM_IDLE;
            break;

        case FF_SM_IDLE:
            if (!interlock) {
                ffSM.curCol = ffSM.startCol;
                ffSM.curRow = ffSM.startRow;
                ffSM.nIdx   = 0;
                ffSM.sp     = 0;          // clean stack
                ffSM.busy   = true;
                ffSM.done   = false;
                ffSM.ffstate = FF_SM_REVEAL_CURR;   // used exactly once
            }
            break;

        case FF_SM_REVEAL_CURR: {
            // This stat is for us to decide in the futur if we want FF to handle game logic
            // or to have a differen SM handle that logic.
            Cell* cur = &msGame.minefield[ffSM.curCol][ffSM.curRow];
            msMinefield_revealCell(ffSM.curCol, ffSM.curRow);

            // If the start is a number or a mine, we are done. Otherwise descend into neighbors.
            if (msMinefield_isMine(cur) || msMinefield_hasProx(cur)) {
                ffSM.ffstate = FF_SM_DONE;
                // Raise some kind of flag here for other SM logic
            } else {
                ffSM.ffstate = FF_SM_CHECK_ADJ;
            }
        } break;

        case FF_SM_CHECK_ADJ:
            if (ffSM.nIdx >= NUM_OF_ADJ_MINES ||
                (ffSM.nIdx == 0 && msFloodFillSM_allAdjRevealed(ffSM.curCol, ffSM.curRow))) {
                if (!msFloodFillSM_ffStack_pop()) {
                    ffSM.ffstate = FF_SM_DONE;
                } else {
                    ffSM.ffstate = FF_SM_CHECK_ADJ; // resume parent
                }
                break;
            }
            break;

        case FF_SM_DONE:
        default:
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
            ffSM.busy = false;
            ffSM.done = false;
            break;

        case FF_SM_IDLE:
            // no-op
            break;

        case FF_SM_REVEAL_CURR:
            // action handled in transition above to guarantee it runs once
            break;

        case FF_SM_CHECK_ADJ:
            // Process exactly one neighbor slot per tick
            dxCol = ffSM.curCol + dx[ffSM.nIdx];
            dxRow = ffSM.curRow + dy[ffSM.nIdx];
            dxIdx = ++ffSM.nIdx;     // pre-increment so parent resumes at the next slot

            //printf("\t\tdxIdx: %d\n", dxIdx+1);
            //printf("\t\tdxCell at col: %d, row: %d", dxCol+1, dxRow+1);



            // Skip out-of-bounds
            if (!msUtil_isInBounds(dxCol, dxRow)) {
                
                //printf(", OUT OF BOUNDS\n");
                break;
            }
            Cell* dxCell = &msGame.minefield[dxCol][dxRow];

            // Skip already revealed
            if (msMinefield_isRevealed(dxCell)) {
                
                //printf(", ALREADY REVEALED\n", dxCol+1, dxRow+1);
                break;
            }

            // Reveal neighbor
            msMinefield_revealCell(dxCol, dxRow);

            // If neighbor is zero, descend immediately: push current frame and switch focus
            if (dxCell->mineProx == 0) {
                
                //printf(", Prox = 0, added to stack", dxCol+1, dxRow+1);
                if (msFloodFillSM_ffStack_push(ffSM.curCol, ffSM.curRow, dxIdx)) {
                    ffSM.curCol = dxCol;
                    ffSM.curRow = dxRow;
                    ffSM.nIdx   = 0;
                    // stay in CHECK_ADJ; do not go back to REVEAL_CURR
                }
                // If push fails due to overflow, we simply do not descend. You can add logging here if desired.
            }
            //printf("\n");
            break;

        case FF_SM_DONE:
            ffSM.busy = false;
            ffSM.done = true;
            interlock = LOCKED;
            break;

        default:
            break;
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
    while (!ffSM.done ||  nTicks <= 1000) {
        //printf("Tick: %d\n", nTicks);
        //printf("\tCuCol: %d, CuRow: %d\n", ffSM.curCol + 1, ffSM.curRow + 1);
        //printf("\tCell Index: %d\n", ffSM.nIdx + 1);
        //printf("\tStack Position: %d\n", ffSM.sp + 1);
        msFloodFillSM_tick();
        nTicks++;
        //printf("\n");
        //msMinefield_terminalPrintMinefield(PRINT_NO);
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

    while (!ffSM.done ||  nTicks <= 1000) {
        //printf("Tick: %d\n", nTicks);
        //printf("\tCuCol: %d, CuRow: %d\n", ffSM.curCol + 1, ffSM.curRow + 1);
        //printf("\tCell Index: %d\n", ffSM.nIdx + 1);
        //printf("\tStack Position: %d\n", ffSM.sp + 1);
        msFloodFillSM_tick();
        nTicks++;
        //printf("\n");
        //msMinefield_terminalPrintMinefield(PRINT_NO);
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

    while (!ffSM.done ||  nTicks <= 1000) {
        //printf("Tick: %d\n", nTicks);
        //printf("\tCuCol: %d, CuRow: %d\n", ffSM.curCol + 1, ffSM.curRow + 1);
        //printf("\tCell Index: %d\n", ffSM.nIdx + 1);
        //printf("\tStack Position: %d\n", ffSM.sp + 1);
        msFloodFillSM_tick();
        nTicks++;
        //printf("\n");
        //msMinefield_terminalPrintMinefield(PRINT_NO);
    }

    // Check results
    printf("FloodFill finished in %d ticks\n", nTicks);
    nTicks = 0;

    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();
}