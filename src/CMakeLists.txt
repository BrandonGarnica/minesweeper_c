# src/CMakeLists.txt

add_executable(minesweeper main.c)

# Set target output directories for Debug and Release
set_target_properties(minesweeper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

target_link_libraries(minesweeper PRIVATE SDL3::SDL3)