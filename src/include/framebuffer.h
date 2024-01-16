#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

class Framebuffer
{
public:
    Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel);
    ~Framebuffer();

    void drawPixel(float x, float y, unsigned int color, bool interpolate = true);
    void clear(unsigned int color);

private:
    void* framebuffer;
    int fbDescriptor, width, height, bytesPerPixel, stride, fbSize;

    [[nodiscard]] std::vector<unsigned int> interpolation(unsigned int color, float dx, float dy) const;
};
