#include "msMinefield.h"
#include "msGame.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define CELL_EMPTY 0
#define CELL_UPDATE_PROX true
#define GAME_BEG_MF_HEIGHT 9
#define GAME_BEG_MF_WIDTH GAME_BEG_MF_HEIGHT
#define GAME_BEG_NUM_MINES 10

#define PRINT_YES true
#define PRINT_NO !PRINT_YES

uint8_t initGame(Minesweeper *game, uint8_t width, uint8_t height, uint8_t num_mines) {
    game->width = width;
    game->height = height;
    game->numOfMines = num_mines;

    srand(time(NULL));

    // Allocate minefield
    game->minefield = malloc(height * sizeof(Cell *));
    if (game->minefield == NULL) return -1; // Allocation failed

    for (int i = 0; i < height; ++i) {
        game->minefield[i] = malloc(width * sizeof(Cell));
        if (game->minefield[i] == NULL) return -1; // Allocation failed
        // Initialize each cell
        for (int j = 0; j < width; ++j) {
            game->minefield[i][j].mineProx = CELL_EMPTY;
            game->minefield[i][j].isMine = false;
            game->minefield[i][j].isFlagged = false;
            game->minefield[i][j].isHidden = true;
        }
    }
    return 0; // Success
}

uint8_t generateMineLocation(Minesweeper *game, uint8_t safeX, uint8_t safeY) {
    uint8_t width = game->width;
    uint8_t height = game->height;
    uint8_t numMines = game->numOfMines;

    if (numMines > width * height - 9) return 1;

    Mine *locations = malloc(numMines * sizeof(Mine));
    if (!locations) return 2;

    bool *occupied = calloc(width * height, sizeof(bool));
    if (!occupied) {
        free(locations);
        return 3;
    }

    // Pre-mark 3x3 safe zone as occupied
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int x = safeX + dx;
            int y = safeY + dy;
            if (x >= 0 && x < width && y >= 0 && y < height) {
                occupied[y * width + x] = true;
            }
        }
    }

    uint16_t placed = 0;
    while (placed < numMines) {
        uint16_t index = rand() % (width * height);
        if (!occupied[index]) {
            occupied[index] = true;
            locations[placed].x = index % width;
            locations[placed].y = index / width;
            placed++;
        }
    }

    free(occupied);
    game->mineList = locations;
    return 0;
}

void placeMines(Minesweeper *game) {
    uint8_t width = game->width;
    uint8_t height = game->height;
    uint8_t numMines = game->numOfMines;

    for (uint8_t i = 0; i < numMines; i++) {
        uint8_t x = game->mineList[i].x;
        uint8_t y = game->mineList[i].y;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            game->minefield[y][x].isMine = true;
        }
    }
}

void updateMineProx(Minesweeper *game, uint8_t mx, uint8_t my, bool draw) {

    uint8_t width = game->width;
    uint8_t height = game->height;

    const int8_t dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    const int8_t dy[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    for (uint8_t i = 0; i < 8; i++) {
        int8_t nx = mx + dx[i];
        int8_t ny = my + dy[i];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            Cell *neighbor = &game->minefield[ny][nx];
            if (!neighbor->isMine && draw) {
                neighbor->mineProx++;
            } else {
                neighbor->mineProx--;
            }
        }
    }
}

void updateMinefieldProx(Minesweeper *game) {
    uint8_t numMines = game->numOfMines;
    for (uint8_t i = 0; i < game->numOfMines; i++) {
        uint8_t mx = game->mineList[i].x;
        uint8_t my = game->mineList[i].y;
        updateMineProx(game, mx, my, CELL_UPDATE_PROX);
    }
}

void freeMinesweeper(Minesweeper *game) {
    if (game->minefield) {
        for (uint8_t i = 0; i < game->height; ++i) {
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

    game->width = 0;
    game->height = 0;
    game->numOfMines = 0;
}

void msMinefield_updateCell(Minesweeper *game, uint8_t rx, uint8_t ry, uint8_t type) {
    uint8_t width = game->width;
    uint8_t height = game->height;

    // Check bounds
    if (rx >= height || ry >= width) return;
    if (rx < 0 || ry < 0) return;

    Cell *cell = &game->minefield[rx][ry];

    // Do not update if already revealed or flagged
    if (!cell->isHidden || cell->isFlagged) return;

    switch (type) {
        case 0:
            cell->isHidden = false;
            break;
        case 1:
            cell->isFlagged = true;
            break;
        default:
            break;
    }
}

void printMinefield(Minesweeper *game, bool show) {
    for (uint8_t i = 0; i < game->height; ++i) {
        for (uint8_t j = 0; j < game->width; ++j) {
            Cell cell = game->minefield[i][j];

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

    initGame(&msTestGame, GAME_BEG_MF_WIDTH, GAME_BEG_MF_HEIGHT, GAME_BEG_NUM_MINES);
    printMinefield(&msTestGame, PRINT_YES);

    generateMineLocation(&msTestGame, 5, 5);
    placeMines(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    updateMinefieldProx(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateCell(&msTestGame, 5,5,0);
    msMinefield_updateCell(&msTestGame, 3,3,0);
    msMinefield_updateCell(&msTestGame, 7,7,1);
    printMinefield(&msTestGame, PRINT_NO);

    freeMinesweeper(&msTestGame);

    printf("Intermediate Game: \n");
    
    initGame(&msTestGame, 16, 16, 40);
    printMinefield(&msTestGame, PRINT_YES);

    generateMineLocation(&msTestGame, 8, 8);
    placeMines(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    updateMinefieldProx(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateCell(&msTestGame, 8,8,0);
    msMinefield_updateCell(&msTestGame, 3,3,0);
    msMinefield_updateCell(&msTestGame, 12,12,1);
    printMinefield(&msTestGame, PRINT_NO);

    freeMinesweeper(&msTestGame);

    printf("Expert Game: \n");
    
    initGame(&msTestGame, 30, 16, 99);
    printMinefield(&msTestGame, PRINT_YES);

    generateMineLocation(&msTestGame, 15, 8);
    placeMines(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    updateMinefieldProx(&msTestGame);
    printMinefield(&msTestGame, PRINT_YES);

    msMinefield_updateCell(&msTestGame, 8,15,0);
    msMinefield_updateCell(&msTestGame, 3,3,0);
    msMinefield_updateCell(&msTestGame, 5,5,1);
    printMinefield(&msTestGame, PRINT_NO);

    freeMinesweeper(&msTestGame);
}