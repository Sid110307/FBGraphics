cmake_minimum_required(VERSION 3.20)
project(FBGraphics LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(fbGraphicsLib STATIC src/framebuffer.cpp src/drawing.cpp)
target_include_directories(fbGraphicsLib PUBLIC src/include)
target_compile_options(fbGraphicsLib PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra> $<$<CXX_COMPILER_ID:MSVC>:/W4>)

# Main
add_executable(fbGraphics src/main.cpp)
target_link_libraries(fbGraphics PRIVATE fbGraphicsLib)

# SDL2_gfx
file(GLOB gfxSources "tests/SDL2_gfx/*.cpp")
add_executable(SDL2_gfx ${gfxSources})
target_include_directories(SDL2_gfx PRIVATE tests/SDL2_gfx)
target_link_libraries(SDL2_gfx PRIVATE fbGraphicsLib)

# Mandelbrot
file(GLOB mandelbrotSources "tests/Mandelbrot/*.cpp")
add_executable(Mandelbrot ${mandelbrotSources})
target_include_directories(Mandelbrot PRIVATE tests/Mandelbrot)
target_link_libraries(Mandelbrot PRIVATE fbGraphicsLib)

# DOOM
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

file(GLOB doomSources "tests/DOOM/*.c")
add_executable(DOOM ${doomSources})
target_include_directories(DOOM PRIVATE tests/DOOM ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(DOOM PRIVATE fbGraphicsLib SDL2::SDL2 SDL2_mixer::SDL2_mixer)
file(COPY tests/DOOM/DOOM.WAD DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
