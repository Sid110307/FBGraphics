#include "include/drawing.h"
#include "include/utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

int main()
{
    Framebuffer framebuffer(WIDTH, HEIGHT, 4);
    unsigned int color = Colors::RGBA(128, 128, 128, 255);

    while (true)
    {
        if (const char c = getChar(); c == 'c') color = Colors::GREEN;
        else if (c == 'q') break;

        framebuffer.clear(color);
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
        Rect(framebuffer, 300, 300, 100, 100, false).draw(Colors::BLUE);
        Rect(framebuffer, 500, 500, 100, 100, true).draw(Colors::BLACK);
        Circle(framebuffer, 700, 700, 50, false).draw(Colors::CYAN);
        Circle(framebuffer, 800, 800, 50, true).draw(Colors::MAGENTA);
        Triangle(framebuffer, 1000, 100, 900, 300, 1100, 300, false).draw(Colors::YELLOW);
        Triangle(framebuffer, 1200, 100, 1100, 300, 1300, 300, true).draw(Colors::WHITE);

        framebuffer.present(0, 0);
    }
    return 0;
}
