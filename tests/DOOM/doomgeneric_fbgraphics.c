// doomgeneric for FBGraphics

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#include "framebuffer.h"

static void* fb = NULL;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0, s_KeyQueueReadIndex = 0;
static unsigned int s_PositionX = 0, s_PositionY = 0;

static unsigned char convertToDoomKey(const unsigned char scancode)
{
    unsigned char key = 0;

    switch (scancode)
    {
        case 0x9C:
        case 0x1C:
            key = KEY_ENTER;
            break;
        case 0x01:
            key = KEY_ESCAPE;
            break;
        case 0xCB:
        case 0x4B:
            key = KEY_LEFTARROW;
            break;
        case 0xCD:
        case 0x4D:
            key = KEY_RIGHTARROW;
            break;
        case 0xC8:
        case 0x48:
            key = KEY_UPARROW;
            break;
        case 0xD0:
        case 0x50:
            key = KEY_DOWNARROW;
            break;
        case 0x1D:
            key = KEY_FIRE;
            break;
        case 0x39:
            key = KEY_USE;
            break;
        case 0x2A:
        case 0x36:
            key = KEY_RSHIFT;
            break;
        case 0x15:
            key = 'y';
            break;
        default:
            break;
    }

    return key;
}

static void addKeyToQueue(const int pressed, const unsigned char keyCode)
{
    const unsigned char key = convertToDoomKey(keyCode);
    const unsigned short keyData = pressed << 8 | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

#ifndef _WIN32
struct termios orig_termios;
void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO);
    raw.c_cc[VMIN] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#else
void enableRawMode() {}
#endif

void DG_Init()
{
    fb = framebuffer_create(DOOMGENERIC_RESX, DOOMGENERIC_RESY, 4);
    enableRawMode();

    const int argPosX = M_CheckParmWithArgs((char*)"-posx", 1);
    if (argPosX) sscanf(myargv[argPosX + 1], "%d", &s_PositionX);

    const int argPosY = M_CheckParmWithArgs((char*)"-posy", 1);
    if (argPosY) sscanf(myargv[argPosY + 1], "%d", &s_PositionY);
}

static void handleKeyInput()
{
    if (fb)
    {
        const C_Event event = framebuffer_pollEvent(fb);
        if (event.type == Quit)
        {
            printf("Quit event received\n");
            exit(0);
        }

        if (event.type == KeyDown) addKeyToQueue(true, event.key);
        else if (event.type == KeyUp) addKeyToQueue(false, event.key);
    }
}

void DG_DrawFrame()
{
    if (fb)
    {
        for (int y = 0; y < DOOMGENERIC_RESY; ++y)
            for (int x = 0; x < DOOMGENERIC_RESX; ++x)
                framebuffer_drawPixel(
                    fb, (float)(x + s_PositionX), (float)(y + s_PositionY), DG_ScreenBuffer[y * DOOMGENERIC_RESX + x]);
        framebuffer_present(fb, 0, 0);
    }
    handleKeyInput();
}

void DG_SleepMs(const uint32_t ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

uint32_t DG_GetTicksMs()
{
#ifdef _WIN32
    return (uint32_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) return 0;
    const unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];

    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xFF;

    return 1;
}

void DG_SetWindowTitle(const char* title) { printf("Window title: %s\n", title); }

int main(const int argc, char** argv)
{
    doomgeneric_Create(argc, argv);
    while (true) doomgeneric_Tick();

    return 0;
}
