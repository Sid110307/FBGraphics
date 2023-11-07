#include "include/drawing.h"
#include "include/input.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

int main()
{
    Framebuffer framebuffer("/dev/fb0", WIDTH, HEIGHT, 4);

    Line(framebuffer, 100, 100, 200, 100).draw(0x00FF00FF);
    Line(framebuffer, 100, 100, 100, 200).draw(0xFF0000FF);
    Line(framebuffer, 100, 100, 200, 300).draw(0x0000FFFF);
    Line(framebuffer, 100, 100, 200, 400).draw(0x000000FF);
    Line(framebuffer, 100, 100, 200, 500).draw(0xFF00FFFF);
    Line(framebuffer, 100, 100, 200, 600).draw(0x00FFFFFF);
    Line(framebuffer, 100, 100, 200, 700).draw(0xFFFFFFFF);

    Pixel(framebuffer, 100, 100).draw(0x00FF00FF);
    Line(framebuffer, 100, 100, 200, 200).draw(0x0000FFFF);
    Rectangle(framebuffer, 300, 300, 100, 100).draw(0x000000FF);
    Rectangle(framebuffer, 500, 500, 100, 100, true).draw(0x00FFFFFF);
    Circle(framebuffer, 700, 700, 50).draw(0x00FFFFFF);
    Circle(framebuffer, 900, 900, 50, true).draw(0x00FF00FF);
    Triangle(framebuffer, 900, 100, 1000, 200, 1100, 100).draw(0x00FF00FF);
    Triangle(framebuffer, 900, 300, 1000, 400, 1100, 300, true).draw(0x00FFFFFF);

    while (true)
    {
        char c = getChar();
        switch (c)
        {
            case 'd':
                Line(framebuffer, 0, 0, WIDTH, HEIGHT).draw(0x000000FF);
                break;
            case 'c':
                framebuffer.clear(0x00FF00FF);
                break;
            default:
                return EXIT_SUCCESS;
        }
    }
}
