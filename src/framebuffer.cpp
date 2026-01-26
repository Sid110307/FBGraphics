#include "include/framebuffer.h"

#ifndef _WIN32
static bool testBit(const unsigned long* bits, int bit)
{
    return bits[bit / (8 * sizeof(unsigned long))] & (1UL << (bit % (8 * sizeof(unsigned long))));
}

static int openKeyboardEventDevice()
{
    const char* dirPath = "/dev/input";
    DIR* dir = opendir(dirPath);
    if (!dir) return -1;

    int bestFd = -1;
    struct dirent* ent;

    while ((ent = readdir(dir)) != nullptr)
    {
        if (strncmp(ent->d_name, "event", 5) != 0) continue;

        std::string path = std::string(dirPath) + "/" + ent->d_name;
        int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        unsigned long evBits[(EV_MAX + 8 * sizeof(unsigned long)) / (8 * sizeof(unsigned long))] = {};
        unsigned long keyBits[(KEY_MAX + 8 * sizeof(unsigned long)) / (8 * sizeof(unsigned long))] = {};

        if (ioctl(fd, EVIOCGBIT(0, sizeof(evBits)), evBits) < 0)
        {
            close(fd);
            continue;
        }

        if (!testBit(evBits, EV_KEY))
        {
            close(fd);
            continue;
        }

        if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) < 0)
        {
            close(fd);
            continue;
        }

        if (testBit(keyBits, KEY_A) && testBit(keyBits, KEY_Z) && testBit(keyBits, KEY_SPACE) &&
            testBit(keyBits, KEY_ENTER))
        {
            bestFd = fd;
            break;
        }

        close(fd);
    }

    closedir(dir);
    return bestFd;
}
#endif

static int mapKey(const int code)
{
    if (code >= 'A' && code <= 'Z') return 'a' + (code - 'A');
    if (code >= '0' && code <= '9') return code;

    switch (code)
    {
#ifdef _WIN32
        case VK_ESCAPE: return 27;
        case VK_RETURN: return 13;
        case VK_SPACE: return 32;
        case VK_BACK: return 8;
        case VK_TAB: return 9;
        case VK_UP: return 0x80;
        case VK_DOWN: return 0x81;
        case VK_LEFT: return 0x82;
        case VK_RIGHT: return 0x83;
#else
        case KEY_ESC: return 27;
        case KEY_ENTER: return 13;
        case KEY_SPACE: return 32;
        case KEY_BACKSPACE: return 8;
        case KEY_TAB: return 9;
        case KEY_UP: return 0x80;
        case KEY_DOWN: return 0x81;
        case KEY_LEFT: return 0x82;
        case KEY_RIGHT: return 0x83;
        case KEY_A: return 'a';
        case KEY_B: return 'b';
        case KEY_C: return 'c';
        case KEY_D: return 'd';
        case KEY_E: return 'e';
        case KEY_F: return 'f';
        case KEY_G: return 'g';
        case KEY_H: return 'h';
        case KEY_I: return 'i';
        case KEY_J: return 'j';
        case KEY_K: return 'k';
        case KEY_L: return 'l';
        case KEY_M: return 'm';
        case KEY_N: return 'n';
        case KEY_O: return 'o';
        case KEY_P: return 'p';
        case KEY_Q: return 'q';
        case KEY_R: return 'r';
        case KEY_S: return 's';
        case KEY_T: return 't';
        case KEY_U: return 'u';
        case KEY_V: return 'v';
        case KEY_W: return 'w';
        case KEY_X: return 'x';
        case KEY_Y: return 'y';
        case KEY_Z: return 'z';
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';
        case KEY_0: return '0';
#endif
        default: return code >= 0 && code <= 255 ? code : 0;
    }
}

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
        std::cerr << "Error opening framebuffer device \"" << fbPath << "\": " << strerror(errno) << std::endl;
        exit(1);
    }

    framebuffer = mmap(nullptr, fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbDescriptor, 0);
    if (framebuffer == MAP_FAILED)
    {
        std::cerr << "Error mapping framebuffer device: " << strerror(errno) << std::endl;
        exit(1);
    }

    inputFd = openKeyboardEventDevice();
    if (inputFd == -1)
    {
        std::cerr << "Error opening keyboard event device" << std::endl;
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
    if (inputFd != -1) close(inputFd);
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

Event Framebuffer::pollEvent()
{
#ifdef _WIN32
    for (int vk = 0; vk < 256; ++vk)
    {
        const bool downNow = (GetAsyncKeyState(vk) & 0x8000) != 0;
        if (const bool downPrev = prevKeyStates[vk]; downNow != downPrev)
        {
            prevKeyStates[vk] = downNow;
            const int key = mapKey(vk);

            if (key >= 0 && key < 256) this->keyStates[key] = downNow;
            if (vk == VK_ESCAPE && downNow) return {EventType::Quit, key};

            return {downNow ? EventType::KeyDown : EventType::KeyUp, key};
        }
    }

    return {EventType::None, 0};
#else
    if (inputFd < 0) return {EventType::None, 0};

    input_event ev{};
    const ssize_t n = read(inputFd, &ev, sizeof(ev));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return {EventType::None, 0};
        return {EventType::None, 0};
    }

    if (n != sizeof(ev)) return {EventType::None, 0};
    if (ev.type != EV_KEY) return {EventType::None, 0};

    const int key = mapKey(ev.code);
    if (key < 0 || key >= 256) return {EventType::None, 0};

    if (ev.value == 1)
    {
        this->keyStates[key] = true;
        if (key == 27) return {EventType::Quit, key};

        return {EventType::KeyDown, key};
    }

    if (ev.value == 0)
    {
        this->keyStates[key] = false;
        return {EventType::KeyUp, key};
    }

    this->keyStates[key] = true;
    return {EventType::KeyDown, key};
#endif
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

void framebuffer_clear(void* instance, const unsigned int color) { static_cast<Framebuffer*>(instance)->clear(color); }

C_Event framebuffer_pollEvent(void* instance)
{
    auto [type, key] = static_cast<Framebuffer*>(instance)->pollEvent();
    return {static_cast<C_EventType>(type), key};
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
