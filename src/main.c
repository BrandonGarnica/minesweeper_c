#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "msGame.h"

int main(int argc, char* argv[]) {
    
    // Init game
    msGame_init();

    while(1) {
        // Tick msControl here
        msGame_tick();
    }
    return 0;
}