cmake_minimum_required(VERSION 3.16)

project(minesweeper C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add src and include directories
add_subdirectory(src)

include_directories(${CMAKE_SOURCE_DIR}/includes)
