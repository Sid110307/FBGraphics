#include "include/framebuffer.h"

Framebuffer::Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel)
        : fb(fbPath, std::ios::in | std::ios::out | std::ios::binary)
{
    width = _width;
    height = _height;
    bytesPerPixel = _bytesPerPixel;
    stride = _width * _bytesPerPixel;
    fbSize = stride * _height;
}

Framebuffer::~Framebuffer() { fb.close(); }

void Framebuffer::drawPixel(float x, float y, unsigned int color)
{
    if (x < 0 || x >= static_cast<float>(width) || y < 0 || y >= static_cast<float>(height)) return;

    auto* interpolatedColor = interpolate(color, x - std::floor(x), y - std::floor(y));

    unsigned int colorBytes[bytesPerPixel];
    colorBytes[0] = interpolatedColor[0] & 0xFF;
    colorBytes[1] = interpolatedColor[1] & 0xFF;
    colorBytes[2] = interpolatedColor[2] & 0xFF;
    colorBytes[3] = interpolatedColor[3] & 0xFF;

    fb.seekp(static_cast<int>(y) * stride + static_cast<int>(x) * bytesPerPixel);
    fb.write(reinterpret_cast<char*>(colorBytes), bytesPerPixel);

    delete[] interpolatedColor;
}

void Framebuffer::clear(unsigned int color)
{
    unsigned int colorBytes[bytesPerPixel];
    colorBytes[0] = color & 0xFF;
    colorBytes[1] = (color >> 8) & 0xFF;
    colorBytes[2] = (color >> 16) & 0xFF;
    colorBytes[3] = (color >> 24) & 0xFF;

    for (int y = 0; y < height; ++y)
    {
        fb.seekp(y * stride);
        for (int x = 0; x < width; ++x) fb.write(reinterpret_cast<char*>(colorBytes), bytesPerPixel);
    }
}

unsigned int* Framebuffer::interpolate(const unsigned int color, float dx, float dy) const
{
    auto* interpolatedColor = new unsigned int[bytesPerPixel];
    unsigned int colorBytes[bytesPerPixel];

    colorBytes[0] = color & 0xFF;
    colorBytes[1] = (color >> 8) & 0xFF;
    colorBytes[2] = (color >> 16) & 0xFF;
    colorBytes[3] = (color >> 24) & 0xFF;

    for (int i = 0; i < bytesPerPixel; ++i)
        interpolatedColor[i] = static_cast<unsigned int>((1.0 - dx) * (1.0 - dy) * colorBytes[i] +
                                                         dx * (1.0 - dy) * colorBytes[i] +
                                                         (1.0 - dx) * dy * colorBytes[i] +
                                                         dx * dy * static_cast<float>(colorBytes[i]));

    return interpolatedColor;
}
