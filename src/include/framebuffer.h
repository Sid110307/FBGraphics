#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __cplusplus

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

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
    Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel);
    ~Framebuffer();

    void drawPixel(float x, float y, unsigned int color, bool interpolate = true);
    [[nodiscard]] unsigned int getPixel(int x, int y) const;
    void clear(unsigned int color);

    [[nodiscard]] Event pollEvent() const;
    void setKeyState(int key, bool state);
    [[nodiscard]] bool getKeyState(int key) const;

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

private:
    void* framebuffer;
    int fbDescriptor, width, height, bytesPerPixel, stride, fbSize;
    bool keyStates[256] = {};

    [[nodiscard]] std::vector<unsigned int> interpolation(unsigned int color, float dx, float dy) const;
};

extern "C" {
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

void* framebuffer_create(const char* fbPath, int width, int height, int bytesPerPixel);
void framebuffer_destroy(void* instance);

void framebuffer_drawPixel(void* instance, float x, float y, unsigned int color, int interpolate);
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
