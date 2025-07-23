# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use SDL3 provided by the system (MSYS2 UCRT64)
find_package(SDL3 REQUIRED CONFIG)

add_subdirectory(src)