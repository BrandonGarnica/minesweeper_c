cmake_minimum_required(VERSION 3.15)
project(minesweeper_c C)

set(CMAKE_C_STANDARD 11)

# Add all source files
file(GLOB SRC_FILES "src/*.c")

# Include headers
include_directories(src)

# Add SDL dependencies
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

# Executable target
add_executable(minesweeper ${SRC_FILES})

# Link SDL
target_link_libraries(minesweeper PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Set output dir
set_target_properties(minesweeper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release
)