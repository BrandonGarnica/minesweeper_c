#define SDL_MAIN_HANDLED

#include "msFloodFillSM.h"
#include "msGameSM.h"
#include "msMinefield.h"

#include <SDL.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {

    //msMinefield_test();
    msFloodFillSM_test();
    return 0;
}