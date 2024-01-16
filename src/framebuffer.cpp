#include "include/framebuffer.h"

Framebuffer::Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel)
        : fbDescriptor(open(fbPath.c_str(), O_RDWR))
{
    if (fbDescriptor == -1)
    {
        std::cerr << "Error opening framebuffer device \"" << fbPath << "\": " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    width = _width;
    height = _height;
    bytesPerPixel = _bytesPerPixel;
    stride = _width * _bytesPerPixel;
    fbSize = stride * _height;

    framebuffer = mmap(nullptr, fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbDescriptor, 0);
    if (framebuffer == MAP_FAILED)
    {
        std::cerr << "Error mapping framebuffer device: " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

Framebuffer::~Framebuffer()
{
    munmap(framebuffer, fbSize);
    close(fbDescriptor);
}

void Framebuffer::drawPixel(float x, float y, unsigned int color, bool interpolate)
{
    if (x < 0 || x >= static_cast<float>(width) || y < 0 || y >= static_cast<float>(height)) return;

    unsigned int* pixelLocation =
            reinterpret_cast<unsigned int*>(framebuffer) + static_cast<int>(y) * width + static_cast<int>(x);

    if (interpolate)
    {
        *pixelLocation = *interpolation(color, x - std::floor(x), y - std::floor(y)).data();
    } else *pixelLocation = color;
}

void Framebuffer::clear(unsigned int color)
{
    auto* pixelLocation = static_cast<unsigned int*>(framebuffer);

    for (int i = 0; i < fbSize / bytesPerPixel; ++i)
    {
        *pixelLocation = color;
        ++pixelLocation;
    }
}

std::vector<unsigned int> Framebuffer::interpolation(const unsigned int color, float dx, float dy) const
{
    std::vector<unsigned int> interpolatedColor(bytesPerPixel);

    for (int i = 0; i < bytesPerPixel; ++i)
    {
        auto colorByte = static_cast<float>((color >> (i * 8)) & 0xFF);
        interpolatedColor[i] = static_cast<unsigned char>(colorByte * (1 - dx) * (1 - dy) + colorByte * dx * (1 - dy) +
                                                          colorByte * (1 - dx) * dy + colorByte * dx * dy);
    }

    return interpolatedColor;
}
