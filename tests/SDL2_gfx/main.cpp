#include "graphics.h"
#include "utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

static void sleep()
{
#ifdef _WIN32
    Sleep(1000);
#else
    usleep(1000000);
#endif
}

int main()
{
    Framebuffer fb(WIDTH, HEIGHT, 4);
    auto step = [&](auto drawFn)
    {
        fb.clear(Colors::BLACK);
        drawFn();

        fb.present(0, 0);
        sleep();

        if (const char c = getChar(); c == 'q') std::exit(0);
    };

    while (true)
    {
        step([&] { pixelColor(fb, 100, 100, Colors::RED); });
        step([&] { hlineColor(fb, 100, 200, 100, Colors::GREEN); });
        step([&] { vlineColor(fb, 100, 100, 200, Colors::BLUE); });
        step([&] { rectangleColor(fb, 100, 100, 200, 200, Colors::CYAN); });
        step([&] { roundedRectangleColor(fb, 100, 100, 200, 200, 10, Colors::MAGENTA); });
        step([&] { boxColor(fb, 100, 100, 200, 200, Colors::YELLOW); });
        step([&] { roundedBoxColor(fb, 100, 100, 200, 200, 10, Colors::WHITE); });
        step([&] { lineColor(fb, 100, 100, 200, 200, Colors::RED); });
        step([&] { aalineColor(fb, 100, 100, 200, 200, Colors::GREEN); });
        step([&] { circleColor(fb, 100, 100, 50, Colors::BLUE); });
        step([&] { arcColor(fb, 100, 100, 50, 0, 90, Colors::CYAN); });
        step([&] { aacircleColor(fb, 100, 100, 50, Colors::MAGENTA); });
        step([&] { ellipseColor(fb, 100, 100, 50, 25, Colors::YELLOW); });
        step([&] { filledCircleColor(fb, 100, 100, 50, Colors::WHITE); });
        step([&] { aaellipseColor(fb, 100, 100, 50, 25, Colors::RED); });
        step([&] { filledEllipseColor(fb, 100, 100, 50, 25, Colors::GREEN); });
        step([&] { pieColor(fb, 100, 100, 50, 0, 90, Colors::BLUE); });
        step([&] { filledPieColor(fb, 100, 100, 50, 0, 90, Colors::CYAN); });
        step([&] { trigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::MAGENTA); });
        step([&] { aatrigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::YELLOW); });
        step([&] { filledTrigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::WHITE); });

        constexpr short x[] = {100, 200, 300, 350};
        constexpr short y[] = {300, 400, 200, 600};
        step([&] { polygonColor(fb, x, y, 4, Colors::RED); });
        step([&] { aapolygonColor(fb, x, y, 4, Colors::GREEN); });
        step([&] { filledPolygonColor(fb, x, y, 4, Colors::BLUE); });
        step([&] { bezierColor(fb, x, y, 4, 10, Colors::CYAN); });
        step([&] { thickLineColor(fb, 100, 100, 200, 200, 10, Colors::MAGENTA); });
    }
    return 0;
}
