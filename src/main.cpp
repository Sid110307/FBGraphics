#include "include/drawing.h"
#include "include/input.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;
constexpr unsigned int RED = 0xFF0000FF, GREEN = 0x00FF00FF, BLUE = 0x0000FFFF, WHITE = 0xFFFFFFFF, BLACK = 0x000000FF,
        YELLOW = 0xFFFF00FF, CYAN = 0x00FFFFFF, MAGENTA = 0xFF00FFFF;

int main()
{
    Framebuffer framebuffer("/dev/fb0", WIDTH, HEIGHT, 4);

    Line(framebuffer, 100, 100, 100, 100).draw(RED);
    Line(framebuffer, 100, 100, 200, 200).draw(GREEN);
    Line(framebuffer, 100, 100, 300, 300).draw(BLUE);
    Line(framebuffer, 100, 100, 400, 400).draw(BLACK);
    Line(framebuffer, 100, 100, 500, 500).draw(YELLOW);
    Line(framebuffer, 100, 100, 600, 600).draw(CYAN);
    Line(framebuffer, 100, 100, 700, 700).draw(MAGENTA);
    Line(framebuffer, 100, 100, 800, 800).draw(WHITE);

    Pixel(framebuffer, 100, 100).draw(RED);
    Line(framebuffer, 100, 100, 200, 200).draw(GREEN);
    Rectangle(framebuffer, 300, 300, 100, 100).draw(BLUE);
    Rectangle(framebuffer, 500, 500, 100, 100, true).draw(BLACK);
    Circle(framebuffer, 700, 700, 50).draw(YELLOW);
    Circle(framebuffer, 900, 900, 50, true).draw(CYAN);
    Triangle(framebuffer, 900, 100, 1000, 200, 1100, 100).draw(MAGENTA);
    Triangle(framebuffer, 900, 300, 1000, 400, 1100, 300, true).draw(WHITE);

    while (true)
    {
        char c = getChar();
        switch (c)
        {
            case 'd':
                Line(framebuffer, 0, 0, WIDTH, HEIGHT).draw(RED);
                Line(framebuffer, 0, HEIGHT, WIDTH, 0).draw(RED);

                break;
            case 'c':
                framebuffer.clear(GREEN);
                break;
            default:
                return EXIT_SUCCESS;
        }
    }
}
