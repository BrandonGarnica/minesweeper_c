# Add executable target
add_executable(minesweeper main.c)

# Include headers
target_include_directories(minesweeper PRIVATE ${CMAKE_SOURCE_DIR}/includes)
