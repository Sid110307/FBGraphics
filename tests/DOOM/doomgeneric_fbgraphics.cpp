//doomgeneric for FBGraphics

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <thread>
#include <chrono>

#include <framebuffer.h>
#include <utils.h>

/*
class Framebuffer
{
public:
    Framebuffer(const std::string &fbPath, int _width, int _height, int _bytesPerPixel);
    ~Framebuffer();

    void drawPixel(float x, float y, unsigned int color, bool interpolate = true);
    [[nodiscard]] unsigned int getPixel(int x, int y) const;
    void clear(unsigned int color);

    void setMousePosition(unsigned int x, unsigned int y);
    [[nodiscard]] unsigned int getMouseX() const;
    [[nodiscard]] unsigned int getMouseY() const;

    void setKeyState(int key, bool state);
    [[nodiscard]] bool getKeyState(int key) const;

    [[nodiscard]] Event pollEvent() const;

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

private:
    ...
}
*/
static Framebuffer* fb = nullptr;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0, s_KeyQueueReadIndex = 0;
static unsigned int s_PositionX = 0, s_PositionY = 0;

static unsigned char convertToDoomKey(unsigned char scancode)
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

static void addKeyToQueue(int pressed, unsigned char keyCode)
{
    unsigned char key = convertToDoomKey(keyCode);
    unsigned short keyData = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

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

void DG_Init()
{
    fb = new Framebuffer("/dev/fb0", DOOMGENERIC_RESX, DOOMGENERIC_RESY, 4);
    enableRawMode();

    int argPosX = M_CheckParmWithArgs((char*) "-posx", 1);
    if (argPosX) sscanf(myargv[argPosX + 1], "%d", &s_PositionX);
//    if (argPosX) s_PositionX = strtol(myargv[argPosX + 1], nullptr, 10);

    int argPosY = M_CheckParmWithArgs((char*) "-posy", 1);
    if (argPosY) sscanf(myargv[argPosY + 1], "%d", &s_PositionY);
//    if (argPosY) s_PositionY = strtol(myargv[argPosY + 1], nullptr, 10);
}

static void handleKeyInput()
{
    if (fb)
    {
        Event event = fb->pollEvent();

        if (event.type == EventType::Quit)
        {
            std::cout << "Quit event received" << std::endl;

            delete fb;
            exit(EXIT_SUCCESS);
        }

        if (event.type == EventType::KeyDown) addKeyToQueue(true, event.key);
        else if (event.type == EventType::KeyUp) addKeyToQueue(false, event.key);
    }
}

void DG_DrawFrame()
{
    if (fb)
        for (int i = 0; i < fb->getHeight(); ++i)
            fb->drawPixel(static_cast<float>(s_PositionX), static_cast<float>(i + s_PositionY),
                          DG_ScreenBuffer[i * DOOMGENERIC_RESX]);

    handleKeyInput();
}

void DG_SleepMs(uint32_t ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

uint32_t DG_GetTicksMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) return 0;
    else
    {
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

        *pressed = keyData >> 8;
        *doomKey = keyData & 0xFF;

        return 1;
    }
}

void DG_SetWindowTitle(const char* title) { std::cout << "Title: " << title << std::endl; }

int main(int argc, char** argv)
{
    doomgeneric_Create(argc, argv);
    while (true) doomgeneric_Tick();
}
