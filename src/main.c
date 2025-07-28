#define SDL_MAIN_HANDLED

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "msGame.h"
#include "msMinefield.h"
#include "msFloodFill.h"

int main(int argc, char* argv[]) {

    msMinefield_test();
    msFloodFill_test();

    return 0;
}