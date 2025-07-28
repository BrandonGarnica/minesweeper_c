#include "msMinefield.h"
#include "msGame.h"
#include "msUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// This function will floodfill reveal all nearby cells when the orginal cell is a 0.
// It is assumed that we are already checking a cell that has been revealed & and is a 0
void msFloodFill_FF(Minesweeper *game, uint8_t col, uint8_t row) {
    uint8_t numCols = game->nCols;
    uint8_t numRows = game->nRows;

    if (!msUtil_isInBounds(game, col, row)) // Condition for going out of bounds
        return;

    Cell *cell = &game->minefield[col][row];

    // Skip already revealecells
    if (!cell->isHidden)
        return;

    cell->isHidden = false;

    // Stop if cell has a number or is a mine
    if (cell->mineProx > 0 || cell->isMine)
        return;

    // Recursive flood fill for 8 directions
    const int8_t dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    const int8_t dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (uint8_t i = 0; i < 8; i++) {
        int8_t newCol = col + dx[i];
        int8_t newRow = row + dy[i];
        // if (newRow >= 0 && newRow < numRows && newCol >= 0 && newCol < numCols)
        if (msUtil_isInBounds(game, newCol, newRow))
            msFloodFill_FF(game, newCol, newRow);
    }
}

void msFloodFill_test() {
    Minesweeper msFloodFillTestgame;

    printf("Beginner Game: \n");
    printf("Flood Fill Test:\n");

    msMinefield_initGame(&msFloodFillTestgame, 9, 9, 10);
    msMinefield_generateMineLocation(&msFloodFillTestgame, 4, 4);
    msMinefield_updateMinefieldProx(&msFloodFillTestgame);

    printf("Before Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);

    printf("Show Minefield:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_YES);

    printf("Flood Fill:\n");
    msFloodFill_FF(&msFloodFillTestgame, 4, 4); // Try to flood from a 0 cell

    printf("After Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);

    msMinefield_freeMinesweeper(&msFloodFillTestgame);

    printf("Intermediate Game: \n");

    msMinefield_initGame(&msFloodFillTestgame, 16, 16, 40);
    msMinefield_generateMineLocation(&msFloodFillTestgame, 8, 8);
    msMinefield_updateMinefieldProx(&msFloodFillTestgame);

    printf("Before Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_YES);

    msFloodFill_FF(&msFloodFillTestgame, 8, 8); // Try to flood from a 0 cell

    printf("After Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);

    msMinefield_freeMinesweeper(&msFloodFillTestgame);

    printf("Expert Game: \n");

    msMinefield_initGame(&msFloodFillTestgame, 30, 16, 99);
    msMinefield_generateMineLocation(&msFloodFillTestgame, 15, 8);
    msMinefield_updateMinefieldProx(&msFloodFillTestgame);

    printf("Before Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_YES);

    msFloodFill_FF(&msFloodFillTestgame, 15, 8); // Try to flood from a 0 cell

    printf("After Flood Fill:\n");
    msMinefield_printMinefield(&msFloodFillTestgame, PRINT_NO);

    msMinefield_freeMinesweeper(&msFloodFillTestgame);
}