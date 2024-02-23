cmake_minimum_required(VERSION 3.20)
project(FBGraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCES src/framebuffer.cpp src/drawing.cpp)
add_executable(fbGraphics src/main.cpp ${SOURCES})

# SDL2_gfx
file(GLOB gfxSources "tests/SDL2_gfx/*.cpp")
add_executable(SDL2_gfx ${gfxSources} ${SOURCES})
target_include_directories(SDL2_gfx PUBLIC tests/SDL2_gfx src/include)

# DOOM
file(GLOB doomSources "tests/DOOM/linuxdoom-1.10/*.c")
add_executable(DOOM ${doomSources} ${SOURCES})
target_include_directories(DOOM PUBLIC tests/DOOM/linuxdoom-1.10 src/include)
target_link_libraries(DOOM m)
