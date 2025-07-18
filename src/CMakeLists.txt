add_executable(minesweeper main.c)

set_target_properties(minesweeper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

target_include_directories(minesweeper PRIVATE ${SDL2_SOURCE_DIR}/include)
target_link_libraries(minesweeper PRIVATE SDL2-static)