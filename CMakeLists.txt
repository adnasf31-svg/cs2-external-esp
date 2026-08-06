cmake_minimum_required(VERSION 3.20)
project(cs2-external-esp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/**/*.cpp"
    "src/**/*.hpp"
)

# Add executable
add_executable(cs2-external-esp ${SOURCES})

# Include directories
target_include_directories(cs2-external-esp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external"
)

# Link libraries
target_link_libraries(cs2-external-esp PRIVATE
    ws2_32
    winmm
    imm32
    ole32
    oleaut32
    shell32
    comctl32
)

# Set output directory
set_target_properties(cs2-external-esp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
