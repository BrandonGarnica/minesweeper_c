#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "msGameSM.h"
#include "msMinefield.h"
#include "msFloodFillSM.h"

int main(int argc, char* argv[]) {

    msMinefield_test();
    msFloodFill_test();

    return 0;
}