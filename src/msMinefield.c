#include "msMinefield.h"
#include "msGame.h"
#include "msUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

uint8_t msMinefield_initGame(Minesweeper *game, uint8_t numCols, uint8_t numRows, uint8_t numMines) {
    game->nCols = numCols;      // Width
    game->nRows = numRows;      // Height
    game->nMines = numMines;   // num_mines

    srand(time(NULL));

    // Allocate outer array of column pointers
    game->minefield = malloc(numCols * sizeof(Cell *));
    if (game->minefield == NULL) return -1;

    // Allocate each column with 'numRows' rows
    for (int col = 0; col < numCols; ++col) {
        game->minefield[col] = malloc(numRows * sizeof(Cell));
        if (game->minefield[col] == NULL) return -1;

        // Initialize each cell in this column
        for (int row = 0; row < numRows; ++row) {
            game->minefield[col][row].mineProx = CELL_EMPTY;
            game->minefield[col][row].isMine = false;
            game->minefield[col][row].isFlagged = false;
            game->minefield[col][row].isHidden = true;
        }
    }
    
    return 0; // Success
}

uint8_t msMinefield_generateMineLocation(Minesweeper *game, uint8_t safeCol, uint8_t safeRow) {
    uint8_t numCols = game->nCols;
    uint8_t numRows = game->nRows;
    uint8_t numMines = game->nMines;

    if (numMines > numCols * numRows) return 1;

    Mine *locations = malloc(numMines * sizeof(Mine));
    if (!locations) return 2;

    bool *occupied = calloc(numCols * numRows, sizeof(bool));
    if (!occupied) {
        free(locations);
        return 3;
    }

    // Mark surrounding area of avoidCol/avoidRow as unavailable
    const int8_t dx[9] = { -1, -1, -1,  0, 0, 0,  1, 1, 1 };
    const int8_t dy[9] = { -1,  0,  1, -1, 0, 1, -1, 0, 1 };

    for (uint8_t i = 0; i < 9; i++) {
        int8_t dxCol = safeCol + dx[i];
        int8_t dxRow = safeRow + dy[i];
        if (msUtil_isInBounds(game, dxRow, dxRow))
            occupied[dxRow * numCols + dxCol] = true;
    }

    // Random placement
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

    game->mineList = locations;

    // Write mines to game->minefield
    for (uint8_t i = 0; i < numMines; i++) {
        uint8_t col = locations[i].col;
        uint8_t row = locations[i].row;
        game->minefield[col][row].isMine = true;
    }

    free(occupied);
    return 0;
}

void msMinefield_updateMineProx(Minesweeper *game, uint8_t col, uint8_t row) {

    const int8_t dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    const int8_t dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (uint8_t i = 0; i < CELL_NUM_OF_SURROUNDING_CELLS; i++) {
        int8_t dxCol = col + dx[i];
        int8_t dxRow = row + dy[i];

        if (msUtil_isInBounds(game, dxCol, dxRow)) {
            Cell *neighbor = &game->minefield[dxCol][dxRow];
            if (!neighbor->isMine) {
                neighbor->mineProx++;
            }
        }
    }
}

void msMinefield_updateMinefieldProx(Minesweeper *game) {

    for (uint8_t i = 0; i < game->nMines; i++) {
        msMinefield_updateMineProx(game, game->mineList[i].col, game->mineList[i].row);
    }
}

void msMinefield_toggleFlagCell(Minesweeper *game, uint8_t col, uint8_t row) {

    if(!msUtil_isInBounds(game, col, row))
        return;
    
     Cell *cell = &game->minefield[col][row];

    // Don't allow toggling flags on already revealed cells
    if (!cell->isHidden)
        return;

    cell->isFlagged = !cell->isFlagged;
}

void msMinefield_revealCell(Minesweeper *game, uint8_t col, uint8_t row) {

    if(!msUtil_isInBounds(game, col, row))
        return;
    
    Cell *cell = &game->minefield[col][row];

    // Don't reveal flagged or already revealed cells
    if (!cell->isHidden || cell->isFlagged)
        return;

    cell->isHidden = false;
}

void msMinefield_freeMinesweeper(Minesweeper *game) {

    uint8_t numRows = game->nRows;

    if (game->minefield) {
        for (uint8_t i = 0; i < numRows; ++i) {
            if (game->minefield[i]) {
                free(game->minefield[i]);
            }
        }
        free(game->minefield);
        game->minefield = NULL;
    }

    if (game->mineList) {
        free(game->mineList);
        game->mineList = NULL;
    }

    game->nCols = 0;
    game->nRows = 0;
    game->nMines = 0;
}

void msMinefield_printMinefield(Minesweeper *game, bool show) {

    for (uint8_t row = 0; row < game->nRows; ++row) {

        for (uint8_t col = 0; col < game->nCols; ++col) {
            Cell cell = game->minefield[col][row];

            if (!cell.isHidden || show) {

                if (cell.isMine) {
                    printf("* ");
                } else {
                    printf("%d ", cell.mineProx);
                }
            } else {

                if (cell.isFlagged) {
                    printf("F ");
                } else {
                    printf("# ");
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void msMinefield_test() {

    Minesweeper msTestGame;

    printf("msMinefield.c Test: \n");

    printf("Beginner Game: \n");

    msMinefield_initGame(&msTestGame, GAME_BEG_MF_NUM_COLS, GAME_BEG_MF_NUM_ROWS, GAME_BEG_NUM_MINES);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_generateMineLocation(&msTestGame, 5, 5);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateMinefieldProx(&msTestGame);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_revealCell(&msTestGame, 5,5);
    msMinefield_revealCell(&msTestGame, 3,3);
    msMinefield_toggleFlagCell(&msTestGame, 7,7);
    msMinefield_printMinefield(&msTestGame, PRINT_NO);

    msMinefield_freeMinesweeper(&msTestGame);

    printf("Intermediate Game: \n");
    
    msMinefield_initGame(&msTestGame, 16, 16, 40);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_generateMineLocation(&msTestGame, 8, 8);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateMinefieldProx(&msTestGame);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_revealCell(&msTestGame, 8,8);
    msMinefield_revealCell(&msTestGame, 10,10);
    msMinefield_toggleFlagCell(&msTestGame, 12,12);
    msMinefield_printMinefield(&msTestGame, PRINT_NO);

    msMinefield_freeMinesweeper(&msTestGame);

    printf("Expert Game: \n");
    
    msMinefield_initGame(&msTestGame, 30, 16, 99);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_generateMineLocation(&msTestGame, 15, 8);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateMinefieldProx(&msTestGame);
    msMinefield_printMinefield(&msTestGame, PRINT_YES);

    msMinefield_revealCell(&msTestGame, 15,8);
    msMinefield_revealCell(&msTestGame, 18,11);
    msMinefield_toggleFlagCell(&msTestGame, 20,14);
    msMinefield_printMinefield(&msTestGame, PRINT_NO);

    msMinefield_freeMinesweeper(&msTestGame);
}