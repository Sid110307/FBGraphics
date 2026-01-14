#include "include/framebuffer.h"

Framebuffer::Framebuffer(const int _width, const int _height, const int _bytesPerPixel)
{
    width = _width;
    height = _height;
    bytesPerPixel = _bytesPerPixel;
    stride = width * bytesPerPixel;
    fbSize = stride * height;

    memset(keyStates, 0, sizeof(keyStates));

#ifdef _WIN32
    desktopDC = GetDC(nullptr);
    if (!desktopDC)
    {
        std::cerr << "Failed to get desktop device context" << std::endl;
        exit(1);
    }

    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = static_cast<WORD>(bytesPerPixel * 8);
    bmi.bmiHeader.biCompression = BI_RGB;

    bits = malloc(static_cast<size_t>(width) * static_cast<size_t>(height) * bytesPerPixel);
    if (!bits)
    {
        ReleaseDC(nullptr, desktopDC);
        desktopDC = nullptr;

        std::cerr << "Memory allocation for framebuffer failed" << std::endl;
        exit(1);
    }

    framebuffer = bits;
#else
    const std::string fbPath = "/dev/fb0";
    fbDescriptor = open(fbPath.c_str(), O_RDWR);

    if (fbDescriptor == -1)
    {
        std::cerr << "Error opening framebuffer device \"" << fbPath << "\": " << std::strerror(errno) << std::endl;
        exit(1);
    }

    framebuffer = mmap(nullptr, fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbDescriptor, 0);
    if (framebuffer == MAP_FAILED)
    {
        std::cerr << "Error mapping framebuffer device: " << strerror(errno) << std::endl;
        exit(1);
    }
#endif
}

Framebuffer::~Framebuffer()
{
#ifdef _WIN32
    if (bits) free(bits);
    bits = nullptr;
    framebuffer = nullptr;

    if (desktopDC) ReleaseDC(nullptr, desktopDC);
    desktopDC = nullptr;
#else
    munmap(framebuffer, fbSize);
    close(fbDescriptor);
#endif
}

void Framebuffer::present(const int x, const int y) const
{
#ifdef _WIN32
    if (!desktopDC || !bits) return;
    StretchDIBits(desktopDC, x, y, width, height, 0, 0, width, height, bits, &bmi, DIB_RGB_COLORS, SRCCOPY);
#endif
}

void Framebuffer::drawPixel(const float x, const float y, const unsigned int color) const
{
    if (x < 0 || x >= static_cast<float>(width) || y < 0 || y >= static_cast<float>(height)) return;
    *(static_cast<unsigned int*>(framebuffer) + static_cast<int>(y) * width + static_cast<int>(x)) = color;
}

unsigned int Framebuffer::getPixel(const int x, const int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height) return 0;
    return *(static_cast<unsigned int*>(framebuffer) + y * width + x);
}

void Framebuffer::clear(const unsigned int color) const
{
    auto* pixelLocation = static_cast<unsigned int*>(framebuffer);
    for (size_t i = 0; i < fbSize / bytesPerPixel; ++i)
    {
        *pixelLocation = color;
        ++pixelLocation;
    }
}

void Framebuffer::setKeyState(const int key, const bool state) { keyStates[key] = state; }
bool Framebuffer::getKeyState(const int key) const { return keyStates[key]; }

Event Framebuffer::pollEvent() const
{
    // TODO: Implement this
    return {EventType::None, 0};
}

int Framebuffer::getWidth() const { return width; }
int Framebuffer::getHeight() const { return height; }

extern "C"
{
void* framebuffer_create(const int width, const int height, const int bytesPerPixel)
{
    return new Framebuffer(width, height, bytesPerPixel);
}

void framebuffer_destroy(void* instance) { delete static_cast<Framebuffer*>(instance); }

void framebuffer_present(void* instance, const int x, const int y)
{
    static_cast<Framebuffer*>(instance)->present(x, y);
}

void framebuffer_drawPixel(void* instance, const float x, const float y, const unsigned int color)
{
    static_cast<Framebuffer*>(instance)->drawPixel(x, y, color);
}

unsigned int framebuffer_getPixel(void* instance, const int x, const int y)
{
    return static_cast<Framebuffer*>(instance)->getPixel(x, y);
}

void framebuffer_clear(void* instance, const unsigned int color) { static_cast<Framebuffer*>(instance)->clear(color); }

C_Event framebuffer_pollEvent(void* instance)
{
    auto event = static_cast<Framebuffer*>(instance)->pollEvent();
    return {static_cast<C_EventType>(event.type), event.key};
}

void framebuffer_setKeyState(void* instance, const int key, const int state)
{
    static_cast<Framebuffer*>(instance)->setKeyState(key, state != 0);
}

int framebuffer_getKeyState(void* instance, const int key)
{
    return static_cast<Framebuffer*>(instance)->getKeyState(key) ? 1 : 0;
}

int framebuffer_getWidth(void* instance) { return static_cast<Framebuffer*>(instance)->getWidth(); }
int framebuffer_getHeight(void* instance) { return static_cast<Framebuffer*>(instance)->getHeight(); }
}
