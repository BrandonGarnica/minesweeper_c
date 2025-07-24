#ifndef MSGAME_H_
#define MSGAME_H_

#include <stdint.h>

typedef struct {
    uint8_t width;
    uint8_t height;
    uint16_t mines;
} GameDiffSettings;

typedef enum {
    GAME_MENU,
    GAME_PLAY,
    GAME_GAMEOVER,
    GAME_QUIT
} GameState;

typedef enum {
    CONTROL_STATE_INIT,
    CONTROL_STATE_PLAYING,
    CONTROL_STATE_WIN,
    CONTROL_STATE_LOSE
} ControlState;

typedef struct {
    GameState gameState;         // High-level mode (e.g., MENU, PLAY, QUIT)
    ControlState controlState;   // Tracks gameplay progress
    
    uint8_t **minefield;
    uint8_t **statefield;
    uint8_t width;
    uint8_t height;
    uint8_t total_mines;
} MSGame;

void msGame_initGameState(MSGame *game);

void msGame_init(void);

void msGame_tick(void);

void msGame_free(void);

#endif /* MSGAME_H_ */