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

    std::memset(keyStates, 0, sizeof(keyStates));
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
    *pixelLocation = interpolate ? *interpolation(color, x - std::floor(x), y - std::floor(y)).data() : color;
}

unsigned int Framebuffer::getPixel(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    return *(reinterpret_cast<unsigned int*>(framebuffer) + y * width + x);
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

void Framebuffer::setKeyState(int key, bool state) { keyStates[key] = state; }
bool Framebuffer::getKeyState(int key) const { return keyStates[key]; }

Event Framebuffer::pollEvent() const
{
    // TODO: Implement this
    return {EventType::None, 0};
}

int Framebuffer::getWidth() const { return width; }
int Framebuffer::getHeight() const { return height; }

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

extern "C"
{
void* framebuffer_create(const char* fbPath, int width, int height, int bytesPerPixel)
{
    return static_cast<void*>(new Framebuffer(fbPath, width, height, bytesPerPixel));
}
void framebuffer_destroy(void* instance) { delete static_cast<Framebuffer*>(instance); }

void framebuffer_drawPixel(void* instance, float x, float y, unsigned int color, int interpolate)
{
    static_cast<Framebuffer*>(instance)->drawPixel(x, y, color, interpolate != 0);
}
unsigned int framebuffer_getPixel(void* instance, int x, int y)
{
    return static_cast<Framebuffer*>(instance)->getPixel(x, y);
}
void framebuffer_clear(void* instance, unsigned int color) { static_cast<Framebuffer*>(instance)->clear(color); }

C_Event framebuffer_pollEvent(void* instance)
{
    auto event = static_cast<Framebuffer*>(instance)->pollEvent();
    return {static_cast<C_EventType>(event.type), event.key};
}
void framebuffer_setKeyState(void* instance, int key, int state)
{
    static_cast<Framebuffer*>(instance)->setKeyState(key, state != 0);
}
int framebuffer_getKeyState(void* instance, int key)
{
    return static_cast<Framebuffer*>(instance)->getKeyState(key) ? 1 : 0;
}

int framebuffer_getWidth(void* instance) { return static_cast<Framebuffer*>(instance)->getWidth(); }
int framebuffer_getHeight(void* instance) { return static_cast<Framebuffer*>(instance)->getHeight(); }
}
