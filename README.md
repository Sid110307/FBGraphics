# FBGraphics

FBGraphics is a low-level C++ graphics library that writes pixels directly to a framebuffer. It provides drawing primitives and input support, together with sample programs for Mandelbrot rendering, SDL2-style graphics tests, and a DOOM port.

## Build

### Requirements

- CMake 3.20 or newer
- A C++20 compiler
- SDL2 and SDL2_mixer for DOOM

```sh
cmake -S . -B bin
cmake --build bin
```

On Linux, the main framebuffer example may require elevated permission to access the framebuffer device:

```sh
sudo ./bin/fbGraphics
# or can run it by ./run.sh or ./run.bat
```
