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

    unsigned int colorBytes[bytesPerPixel];
    colorBytes[0] = color & 0xFF;
    colorBytes[1] = (color >> 8) & 0xFF;
    colorBytes[2] = (color >> 16) & 0xFF;
    colorBytes[3] = (color >> 24) & 0xFF;

    fb.seekp(static_cast<int>(y) * stride + static_cast<int>(x) * bytesPerPixel);
    fb.write(reinterpret_cast<char*>(colorBytes), bytesPerPixel);

    float dx = x - std::round(x);
    float dy = y - std::round(y);

    if (dx > 0.0f && dy > 0.0f)
    {
        unsigned int* interpolatedColor = interpolate(color, dx, dy);
        fb.seekp(static_cast<int>(y) * stride + static_cast<int>(x) * bytesPerPixel);
        fb.write(reinterpret_cast<char*>(interpolatedColor), bytesPerPixel);
        delete[] interpolatedColor;
    }

    if (dx > 0.0f && dy < 1.0f)
    {
        unsigned int* interpolatedColor = interpolate(color, dx, 1.0f - dy);
        fb.seekp(static_cast<int>(y + 1.0f) * stride + static_cast<int>(x) * bytesPerPixel);
        fb.write(reinterpret_cast<char*>(interpolatedColor), bytesPerPixel);
        delete[] interpolatedColor;
    }

    if (dx < 1.0f && dy > 0.0f)
    {
        unsigned int* interpolatedColor = interpolate(color, 1.0f - dx, dy);
        fb.seekp(static_cast<int>(y) * stride + static_cast<int>(x + 1.0f) * bytesPerPixel);
        fb.write(reinterpret_cast<char*>(interpolatedColor), bytesPerPixel);
        delete[] interpolatedColor;
    }

    if (dx < 1.0f && dy < 1.0f)
    {
        unsigned int* interpolatedColor = interpolate(color, 1.0f - dx, 1.0f - dy);
        fb.seekp(static_cast<int>(y + 1.0f) * stride + static_cast<int>(x + 1.0f) * bytesPerPixel);
        fb.write(reinterpret_cast<char*>(interpolatedColor), bytesPerPixel);
        delete[] interpolatedColor;
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
        interpolatedColor[i] = static_cast<unsigned int>(static_cast<float>(colorBytes[i]) * dx * dy);
    return interpolatedColor;
}
