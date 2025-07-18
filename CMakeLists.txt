cmake_minimum_required(VERSION 3.16)
project(Minesweeper_SDL2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use FetchContent to download SDL2 if not installed locally
include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.2 # Or latest stable tag
)

FetchContent_MakeAvailable(SDL2)

add_subdirectory(src)