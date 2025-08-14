#include "msMinefield.h"
#include "msGameSM.h"
#include "msUtil.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void msMinefield_terminalPrintMinefield(bool reveal) {

    // Print column headers
    printf("   "); // space for row labels
    for (uint8_t col = 0; col < msGame.nCols; ++col) {
        printf("%2d", col + 1);
    }
    printf("\n");

    for (uint8_t row = 0; row < msGame.nRows; ++row) {
        // Print row label
        printf("%2d ", row + 1);

        for (uint8_t col = 0; col < msGame.nCols; ++col) {
            Cell cell = msGame.minefield[col][row];

            if (!cell.isHidden || reveal) {
                if (cell.isMine) {
                    printf(" *");
                } else {
                    printf(" %d", cell.mineProx);
                }
            } else {
                if (cell.isFlagged) {
                    printf(" F");
                } else {
                    printf(" #");
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void msMinefield_toggleFlagCell(uint8_t col, uint8_t row) {

    if (!msUtil_isInBounds(col, row)) {
        return;
    }

    Cell* cell = &msGame.minefield[col][row];

    // Don't allow toggling flags on already revealed cells
    if (!cell->isHidden) {
        return;
    }

    cell->isFlagged = !cell->isFlagged;
    // Draw Flagged icon on cell location
}

void msMinefield_revealCell(uint8_t col, uint8_t row) {

    if (!msUtil_isInBounds(col, row)) {
        return;
    }

    Cell* cell = &msGame.minefield[col][row];

    // Don't reveal flagged or already revealed cells
    if (!cell->isHidden || cell->isFlagged) {
        return;
    }

    cell->isHidden = false;

    // Only propagate counts if this isn't a mine
    if (!cell->isMine) {
        for (int8_t dy = -1; dy <= 1; ++dy) {
            for (int8_t dx = -1; dx <= 1; ++dx) {
                // Skip self
                if (!(dx == 0 && dy == 0)) {
                    int16_t nc = (int16_t)col + dx;
                    int16_t nr = (int16_t)row + dy;

                    if (nc >= 0 && nc < msGame.nCols && nr >= 0 && nr < msGame.nRows) {
                        Cell* n = &msGame.minefield[nc][nr];
                        if (n->adjRevealed < n->adjTotal) {
                            n->adjRevealed++; // idempotent due to isHidden check above
                        }
                    }
                }
            }
        }
    }

    // Draw cell prox or if mine
}

// Changes these 3 functiosn from Cell* Cell to just a col & row
bool msMinefield_isRevealed(uint8_t col, uint8_t row) {
    Cell* rCell = &msGame.minefield[col][row];
    return !rCell->isHidden;
}

bool msMinefield_hasProx(uint8_t col, uint8_t row) {

    Cell* pCell = &msGame.minefield[col][row];
    return pCell->mineProx > 0;
}

bool msMinefield_isMine(uint8_t col, uint8_t row) {

    Cell* mCell = &msGame.minefield[col][row];
    return mCell->isMine;
}

void msMinefield_freeMinesweeper() {

    uint8_t numRows = msGame.nRows;

    if (msGame.minefield) {
        // You looped over rows. You allocated by columns.
        for (uint8_t col = 0; col < msGame.nCols; ++col) {
            free(msGame.minefield[col]);
        }
        free(msGame.minefield);
        msGame.minefield = NULL;
    }
    free(msGame.mineList); // safe if NULL
    msGame.mineList = NULL;
    msGame.nCols = msGame.nRows = msGame.nMines = 0;
}

void msMinefield_updateMineProx(uint8_t col, uint8_t row) {

    for (uint8_t i = 0; i < CELL_NUM_OF_SURROUNDING_CELLS; i++) {
        int8_t dxCol = col + dx[i];
        int8_t dxRow = row + dy[i];

        if (msUtil_isInBounds(dxCol, dxRow)) {
            Cell* neighborCell = &msGame.minefield[dxCol][dxRow];
            if (!neighborCell->isMine) {
                neighborCell->mineProx++;
            }
        }
    }
}

void msMinefield_updateMinefieldProx() {

    for (uint8_t i = 0; i < msGame.nMines; i++) {
        msMinefield_updateMineProx(msGame.mineList[i].col, msGame.mineList[i].row);
    }
}

void msMinefield_generateMineLocation(uint8_t safeCol, uint8_t safeRow) {
    uint8_t numCols = msGame.nCols;
    uint8_t numRows = msGame.nRows;
    uint8_t numMines = msGame.nMines;

    uint8_t reserved = 0;
    for (int8_t dy = -1; dy <= 1; ++dy) {
        for (int8_t dx = -1; dx <= 1; ++dx) {
            if (msUtil_isInBounds(safeCol + dx, safeRow + dy)) {
                reserved++;
            }
        }
    }

    const uint32_t capacity = (uint32_t)numCols * numRows;
    if (numMines > capacity - reserved) {
        return; // impossible request
    }

    if (numMines > numCols * numRows) {
        return; // too many mines for grid size
    }

    Mine* locations = malloc(numMines * sizeof(Mine));
    if (!locations) {
        return; // allocation failed
    }

    bool* occupied = calloc(numCols * numRows, sizeof(bool));
    if (!occupied) {
        free(locations);
        return; // allocation failed
    }

    // Mark safe 3x3 zone around safeCol/safeRow
    for (int8_t dy = -1; dy <= 1; dy++) {
        for (int8_t dx = -1; dx <= 1; dx++) {
            int8_t c = safeCol + dx;
            int8_t r = safeRow + dy;
            if (msUtil_isInBounds(c, r)) {
                occupied[r * numCols + c] = true;
            }
        }
    }

    // Place mines randomly outside safe zone
    uint8_t placed = 0;
    while (placed < numMines) {
        uint16_t index = rand() % (numCols * numRows);
        if (!occupied[index]) {
            occupied[index] = true;
            locations[placed].col = index % numCols;
            locations[placed].row = index / numCols;
            placed++;
        }
    }

    msGame.mineList = locations;

    // Apply mines to the minefield
    for (uint8_t i = 0; i < numMines; i++) {
        msGame.minefield[locations[i].col][locations[i].row].isMine = true;
    }

    free(occupied);

    msMinefield_updateMinefieldProx();
}

void msMinefield_updateAdjTotals_Borders() {
    if (msGame.nCols < 2 || msGame.nRows < 2) {
        // Degenerate sizes: handle elsewhere or compute per cell if needed.
        return;
    }

    // Update corners
    msGame.minefield[0][0].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_CORNER;
    msGame.minefield[msGame.nCols - 1][0].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_CORNER;
    msGame.minefield[0][msGame.nRows - 1].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_CORNER;
    msGame.minefield[msGame.nCols - 1][msGame.nRows - 1].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_CORNER;

    // Top and bottom edges (excluding corners)
    for (uint8_t col = 0; col < msGame.nCols; ++col) {
        msGame.minefield[col][0].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_EDGE;
        msGame.minefield[col][msGame.nRows - 1].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_EDGE;
    }

    // Left and right edges (excluding corners)
    for (uint8_t row = 0; row < msGame.nRows; ++row) {
        msGame.minefield[0][row].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_EDGE;
        msGame.minefield[msGame.nCols - 1][row].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS_EDGE;
    }
}

void msMinefield_initMinefield() {
    srand(time(NULL));

    msGame.minefield = malloc(msGame.nCols * sizeof(Cell*));
    if (msGame.minefield == NULL) {
        return;
    }

    // Allocate each column with 'numRows' rows
    for (uint8_t col = 0; col < msGame.nCols; ++col) {

        msGame.minefield[col] = malloc(msGame.nRows * sizeof(Cell));
        if (msGame.minefield[col] == NULL) {
            // free previously allocated columns
            for (uint8_t c = 0; c < col; ++c) {
                free(msGame.minefield[c]);
            }
            free(msGame.minefield);
            msGame.minefield = NULL;
            return;
        }

        // Initialize each cell in this column
        for (uint8_t row = 0; row < msGame.nRows; ++row) {
            msGame.minefield[col][row].mineProx = CELL_EMPTY;
            msGame.minefield[col][row].adjRevealed = CELL_EMPTY;
            msGame.minefield[col][row].isMine = false;
            msGame.minefield[col][row].isFlagged = false;
            msGame.minefield[col][row].isHidden = true;
            msGame.minefield[col][row].adjTotal = CELL_NUM_OF_SURROUNDING_CELLS;
        }
    }

    msMinefield_updateAdjTotals_Borders();
}

void msMinefield_generateMinefield() { msMinefield_initMinefield(); }

void msMinefield_initParams(uint8_t numCols, uint8_t numRows, uint8_t numMines) {
    msGame.nCols = numCols;
    msGame.nRows = numRows;
    msGame.nMines = numMines;
}

void msMinefield_test() {

    printf("msMinefield.c Test: \n");

    printf("Beginner Game: \n");

    msMinefield_initParams(GAME_BEG_MF_NUM_COLS, GAME_BEG_MF_NUM_ROWS, GAME_BEG_NUM_MINES);
    msMinefield_generateMinefield();
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_generateMineLocation(4, 4);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_revealCell(4, 4);
    msMinefield_revealCell(5, 5);
    msMinefield_toggleFlagCell(7, 7);
    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();
}