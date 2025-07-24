#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "msControl.h"
#include "msDisplay.h"
#include "msFloodFill.h"
#include "msGame.h"
#include "msMenu.h"
#include "msMinefield.h"
#include "msUserInput.h"

#define GAME_DEFAULT_DIFF GAME_DIFF_BEGINNER
#define GAME_DEFAULT_MF_SIZE GAME_BEGINNER_MF_SIZE
#define GAME_DEFAULT_NUM_MINES GAME_BEGINNER_NUM_MINES

#define GAME_DIFF_BEGINNER 0
#define GAME_DIFF_INTERMEDIATE 1
#define GAME_DIFF_EXPERT 2

#define GAME_NUM_DIFF 2

#define GAME_BEGINNER_NUM_MINES 10
#define GAME_BEGINNER_MF_SIZE 9

#define GAME_INTERMEDIATE_NUM_MINES 40
#define GAME_INTERMEDIATE_MF_SIZE 16

#define GAME_EXPERT_NUM_MINES 99
#define GAME_EXPERT_MF_SIZE_H 16
#define GAME_EXPERT_MF_SIZE_W 30

#define GAME_TILE_INIT 0x00
#define GAME_MINE_EMPTY 0

#define TEST_MINEFIELD 1
#define TEST_GAME 4

static MSGame msGame;

void msGame_initGameState(MSGame *game) {
    game->gameState = GAME_MENU;
    game->controlState = CONTROL_STATE_INIT;

    game->width = GAME_DEFAULT_MF_SIZE;
    game->height = GAME_DEFAULT_MF_SIZE;
    game->total_mines = GAME_DEFAULT_NUM_MINES;

    game->minefield = NULL;
    game->statefield = NULL;
}

static const GameDiffSettings diffSettings[] = {
    [GAME_DIFF_BEGINNER]     = {GAME_BEGINNER_MF_SIZE, GAME_BEGINNER_MF_SIZE, GAME_BEGINNER_NUM_MINES},
    [GAME_DIFF_INTERMEDIATE] = {GAME_INTERMEDIATE_MF_SIZE, GAME_INTERMEDIATE_MF_SIZE, GAME_INTERMEDIATE_NUM_MINES},
    [GAME_DIFF_EXPERT]       = {GAME_EXPERT_MF_SIZE_W, GAME_EXPERT_MF_SIZE_H, GAME_EXPERT_NUM_MINES},
};

void msGame_setDiff(MSGame *game, uint8_t difficulty) {
    if (difficulty >= GAME_NUM_DIFF) return; // silently fail or handle error
    game->width = diffSettings[difficulty].width;
    game->height = diffSettings[difficulty].height;
    game->total_mines = diffSettings[difficulty].mines;
}

void msGame_free() {
    // Free all things here
}

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the following lines to run Test 1, 2, 3, or 4    //////
////////////////////////////////////////////////////////////////////////////////
#define RUN_PROGRAM TEST_MINEFIELD
// #define RUN_PROGRAM TEST_GAME

// If nothing is uncommented above, run milestone 4
#ifndef RUN_PROGRAM
#define RUN_PROGRAM TEST_MINEFIELD
#endif

/****************************** TEST_MINEFIELD ****************/
#if RUN_PROGRAM == TEST_MINEFIELD
void msGame_init() {
    msGame_init();
    msMinefield_init(&msGame);
    printf("Running the TEST_MINEFIELD.\n");
}

void msGame_tick() {
}
#endif

/****************************** TEST_MINEFIELD ****************/
#if RUN_PROGRAM == TEST_GAME
static void msGame_init() {
  printf("Running the TEST_MINEFIELD.\n");
}

void msGame_tick() {
    // Tick all SM here
    switch (msGame.gameState)
    {
    case GAME_MENU:
        // Tick msMenu_tick(&msGame);
        break;
    
    case GAME_PLAY:
        // Tick:
        // msMinefield_init
        // msControl_tick(&msGame);
        break;

    case GAME_GAMEOVER:
        // 
        break;

    case GAME_QUIT:
        exit(0);
        break;
    }
}
#endif

