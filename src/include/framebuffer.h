#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef __cplusplus

#include <iostream>

enum class EventType
{
    None,
    Quit,
    KeyDown,
    KeyUp
};

struct Event
{
    EventType type;
    int key;
};

class Framebuffer
{
public:
    Framebuffer(int _width, int _height, int _bytesPerPixel);
    ~Framebuffer();

    void present(int x, int y) const;
    void drawPixel(float x, float y, unsigned int color) const;
    [[nodiscard]] unsigned int getPixel(int x, int y) const;
    void clear(unsigned int color) const;

    [[nodiscard]] Event pollEvent() const;
    void setKeyState(int key, bool state);
    [[nodiscard]] bool getKeyState(int key) const;

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

private:
    void* framebuffer = nullptr;
    int fbDescriptor = -1, width, height, bytesPerPixel, stride, fbSize;
    bool keyStates[256] = {};

#ifdef _WIN32
    HDC desktopDC = nullptr;
    BITMAPINFO bmi = {};
    void* bits = nullptr;
#endif
};

extern "C"
{
#endif

typedef enum
{
    None,
    Quit,
    KeyDown,
    KeyUp
} C_EventType;

typedef struct
{
    C_EventType type;
    int key;
} C_Event;

void* framebuffer_create(int width, int height, int bytesPerPixel);
void framebuffer_destroy(void* instance);

void framebuffer_present(void* instance, int x, int y);
void framebuffer_drawPixel(void* instance, float x, float y, unsigned int color);
unsigned int framebuffer_getPixel(void* instance, int x, int y);
void framebuffer_clear(void* instance, unsigned int color);

C_Event framebuffer_pollEvent(void* instance);
void framebuffer_setKeyState(void* instance, int key, int state);
int framebuffer_getKeyState(void* instance, int key);

int framebuffer_getWidth(void* instance);
int framebuffer_getHeight(void* instance);

#ifdef __cplusplus
}
#endif
