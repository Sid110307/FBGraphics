cmake_minimum_required(VERSION 3.20)
project(FBGraphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCES src/framebuffer.cpp src/drawing.cpp)
add_executable(fbGraphics src/main.cpp ${SOURCES})

file(GLOB exampleDirs "tests/*")
foreach (child ${exampleDirs})
    if (IS_DIRECTORY ${child})
        file(GLOB examples "${child}/*.cpp")

        get_filename_component(childName ${child} NAME)
        add_executable(${childName} ${examples} ${SOURCES})
        target_include_directories(${childName} PUBLIC ${child} src/include)
    endif ()
endforeach (child)
