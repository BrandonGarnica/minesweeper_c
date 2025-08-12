#include "msMinefield.h"
#include "msGameSM.h"
#include "msUtil.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t msMinefield_initGame(uint8_t numCols, uint8_t numRows, uint8_t numMines) {

    msGame.nCols = numCols;
    msGame.nRows = numRows;
    msGame.nMines = numMines;

    srand(time(NULL));
    msGame.minefield = malloc(numCols * sizeof(Cell*));
    if (msGame.minefield == NULL) {
        return -1;
    }

    // Allocate each column with 'numRows' rows
    for (int col = 0; col < numCols; ++col) {

        msGame.minefield[col] = malloc(numRows * sizeof(Cell));
        if (msGame.minefield[col] == NULL) {
            return -1;
        }

        // Initialize each cell in this column
        for (int row = 0; row < numRows; ++row) {
            msGame.minefield[col][row].mineProx = CELL_EMPTY;
            msGame.minefield[col][row].isMine = false;
            msGame.minefield[col][row].isFlagged = false;
            msGame.minefield[col][row].isHidden = true;
        }
    }

    return 0; // Success
}

uint8_t msMinefield_generateMineLocation(uint8_t safeCol, uint8_t safeRow) {
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
        return 1; // impossible request
    }

    if (numMines > numCols * numRows) {
        return 2; // too many mines for grid size
    }

    Mine* locations = malloc(numMines * sizeof(Mine));
    if (!locations) {
        return 3; // allocation failed
    }

    bool* occupied = calloc(numCols * numRows, sizeof(bool));
    if (!occupied) {
        free(locations);
        return 4; // allocation failed
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
    return 0;
}

void msMinefield_updateMineProx(uint8_t col, uint8_t row) {

    for (uint8_t i = 0; i < CELL_NUM_OF_SURROUNDING_CELLS; i++) {
        int8_t dxCol = col + dx[i];
        int8_t dxRow = row + dy[i];

        if (msUtil_isInBounds(dxCol, dxRow)) {
            Cell* neighbor = &msGame.minefield[dxCol][dxRow];
            if (!neighbor->isMine) {
                neighbor->mineProx++;
            }
        }
    }
}

void msMinefield_updateMinefieldProx() {

    for (uint8_t i = 0; i < msGame.nMines; i++) {
        msMinefield_updateMineProx(msGame.mineList[i].col, msGame.mineList[i].row);
    }
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
    // Draw cell prox or if mine
}

// Changes these 3 functiosn from Cell* Cell to just a col & row
bool msMinefield_isRevealed(Cell* cell) { return !cell->isHidden; }

bool msMinefield_hasProx(Cell* cell) { return cell->mineProx > 0; }

bool msMinefield_isMine(Cell* cell) { return cell->isMine; }

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

void msMinefield_terminalPrintMinefield(bool show) {

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

            if (!cell.isHidden || show) {
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

void msMinefield_test() {

    printf("msMinefield.c Test: \n");

    printf("Beginner Game: \n");

    msMinefield_initGame(GAME_BEG_MF_NUM_COLS, GAME_BEG_MF_NUM_ROWS, GAME_BEG_NUM_MINES);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_generateMineLocation(4, 4);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_updateMinefieldProx();
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_revealCell(4, 4);
    msMinefield_revealCell(5, 5);
    msMinefield_toggleFlagCell(7, 7);
    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();

    printf("Intermediate Game: \n");

    msMinefield_initGame(16, 16, 40);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_generateMineLocation(7, 7);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_updateMinefieldProx();
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_revealCell(7, 7);
    msMinefield_revealCell(10, 10);
    msMinefield_toggleFlagCell(12, 12);
    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();

    printf("Expert Game: \n");

    msMinefield_initGame(30, 16, 99);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_generateMineLocation(14, 9);
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_updateMinefieldProx();
    msMinefield_terminalPrintMinefield(PRINT_YES);

    msMinefield_revealCell(14, 9);
    msMinefield_revealCell(18, 11);
    msMinefield_toggleFlagCell(20, 14);
    msMinefield_terminalPrintMinefield(PRINT_NO);

    msMinefield_freeMinesweeper();
}