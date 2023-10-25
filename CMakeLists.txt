cmake_minimum_required(VERSION 3.20)
project(FBGraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(fbGraphics
        src/main.cpp
        src/framebuffer.cpp
        src/drawing.cpp
)

