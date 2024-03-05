#include "include/drawing.h"
#include "include/utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

int main()
{
    Framebuffer framebuffer("/dev/fb0", WIDTH, HEIGHT, 4);

    Line(framebuffer, 1300, 100, 100, 100).draw(Colors::RED);
    Line(framebuffer, 1300, 100, 200, 200).draw(Colors::GREEN);
    Line(framebuffer, 1300, 100, 300, 300).draw(Colors::BLUE);
    Line(framebuffer, 1300, 100, 400, 400).draw(Colors::BLACK);
    Line(framebuffer, 1300, 100, 500, 500).draw(Colors::CYAN);
    Line(framebuffer, 1300, 100, 600, 600).draw(Colors::MAGENTA);
    Line(framebuffer, 1300, 100, 700, 700).draw(Colors::YELLOW);
    Line(framebuffer, 1300, 100, 800, 800).draw(Colors::WHITE);

    Pixel(framebuffer, 100, 100).draw(Colors::RED);
    Line(framebuffer, 100, 100, 200, 200).draw(Colors::GREEN);
    Rectangle(framebuffer, 300, 300, 100, 100).draw(Colors::BLUE);
    Rectangle(framebuffer, 500, 500, 100, 100, true).draw(Colors::BLACK);
    Circle(framebuffer, 700, 700, 50).draw(Colors::CYAN);
    Circle(framebuffer, 900, 900, 50, true).draw(Colors::MAGENTA);
    Triangle(framebuffer, 900, 100, 1000, 200, 1100, 100).draw(Colors::YELLOW);
    Triangle(framebuffer, 900, 300, 1000, 400, 1100, 300, true).draw(Colors::WHITE);

    while (true)
    {
        char c = getChar();
        switch (c)
        {
            case 'd':
                Line(framebuffer, 0, 0, WIDTH, HEIGHT).draw(Colors::RED);
                Line(framebuffer, 0, HEIGHT, WIDTH, 0).draw(Colors::RED);

                break;
            case 'c':
                framebuffer.clear(Colors::GREEN);
                break;
            default:
                return EXIT_SUCCESS;
        }
    }
}
