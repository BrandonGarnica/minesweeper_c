cmake_minimum_required(VERSION 3.15)
project(minesweeper_c C)
set(CMAKE_C_STANDARD 11)

option(USE_SDL "Build with SDL backend" ON)

# Common sources
file(GLOB SRC_FILES "src/*.c")

# Choose backend source
if(USE_SDL)
  message(STATUS "Building with SDL backend")
  list(APPEND SRC_FILES src/msDisplay_SDL.c)
else()
  message(STATUS "Building with Embedded backend")
  list(APPEND SRC_FILES src/msDisplay_Embedded.c)
endif()

# Executable
add_executable(minesweeper ${SRC_FILES})

# Includes
include_directories(src)
include_directories(includes)

# SDL only when enabled
if(USE_SDL)
  find_package(SDL2 REQUIRED CONFIG)
  find_package(SDL2_ttf REQUIRED CONFIG)
  target_link_libraries(minesweeper PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)
endif()

# Output dirs
set_target_properties(minesweeper PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Debug
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)