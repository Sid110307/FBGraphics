#pragma once

#include <fstream>
#include <cmath>

class Framebuffer
{
public:
    Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel);
    ~Framebuffer();

    void drawPixel(float x, float y, unsigned int color);
    void clear(unsigned int color);

private:
    std::fstream fb;
    int width, height, bytesPerPixel, stride, fbSize;

    unsigned int* interpolate(unsigned int color, float dx, float dy) const;
};
