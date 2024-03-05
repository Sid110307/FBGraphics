cmake_minimum_required(VERSION 3.20)
project(FBGraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_library(fbGraphicsLib STATIC src/framebuffer.cpp src/drawing.cpp)
target_include_directories(fbGraphicsLib PUBLIC src/include)

# Main
add_executable(fbGraphics src/main.cpp)
target_link_libraries(fbGraphics PRIVATE fbGraphicsLib)

# SDL2_gfx
file(GLOB gfxSources "tests/SDL2_gfx/*.cpp")
add_executable(SDL2_gfx ${gfxSources})
target_include_directories(SDL2_gfx PUBLIC tests/SDL2_gfx)
target_link_libraries(SDL2_gfx PRIVATE fbGraphicsLib)

# DOOM
file(GLOB doomSources "tests/DOOM/*.c")
add_executable(DOOM ${doomSources})
target_include_directories(DOOM PUBLIC tests/DOOM)
target_link_libraries(DOOM PRIVATE fbGraphicsLib SDL2 SDL2_mixer)
file(COPY tests/DOOM/DOOM.WAD DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Mandelbrot
add_executable(Mandelbrot tests/Mandelbrot/main.cpp)
target_link_libraries(Mandelbrot PRIVATE fbGraphicsLib)
