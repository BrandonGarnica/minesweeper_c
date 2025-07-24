#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "msMinefield.h"
#include "msGame.h"

// Init minefield
void msMinefield_init(MSGame *game) {
    printf("Init msMinefield\n");
}

// Updates the minefield prox value based on number of adjacent mines
void msMinefield_tileProx() {}

// Randomly generates mine locations
void msMinefield_generateMineLocations(uint8_t gameDifficulty) {}

// Toggle flag for tile
void msMinefield_toggleFlag() {}

// Reveal title
void msMinefield_revealTile() {}

// Generates minefield & inits statefield
void msMinefield_generateMinefield() {}