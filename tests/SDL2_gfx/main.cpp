#include <unistd.h>

#include "graphics.h"
#include "utils.h"

constexpr int WIDTH = 640, HEIGHT = 480;
void reset(Framebuffer &fb)
{
    sleep(1);
    fb.clear(Colors::BLACK);
}

int main()
{
    Framebuffer fb("/dev/fb0", WIDTH, HEIGHT, 4);

    pixelColor(fb, 100, 100, Colors::RED);
    reset(fb);

    hlineColor(fb, 100, 200, 100, Colors::GREEN);
    reset(fb);

    vlineColor(fb, 100, 100, 200, Colors::BLUE);
    reset(fb);

    rectangleColor(fb, 100, 100, 200, 200, Colors::CYAN);
    reset(fb);

    roundedRectangleColor(fb, 100, 100, 200, 200, 10, Colors::MAGENTA);
    reset(fb);

    boxColor(fb, 100, 100, 200, 200, Colors::YELLOW);
    reset(fb);

    roundedBoxColor(fb, 100, 100, 200, 200, 10, Colors::WHITE);
    reset(fb);

    lineColor(fb, 100, 100, 200, 200, Colors::RED);
    reset(fb);

    aalineColor(fb, 100, 100, 200, 200, Colors::GREEN);
    reset(fb);

    circleColor(fb, 100, 100, 50, Colors::BLUE);
    reset(fb);

    arcColor(fb, 100, 100, 50, 0, 90, Colors::CYAN);
    reset(fb);

    aacircleColor(fb, 100, 100, 50, Colors::MAGENTA);
    reset(fb);

    ellipseColor(fb, 100, 100, 50, 25, Colors::YELLOW);
    reset(fb);

    filledCircleColor(fb, 100, 100, 50, Colors::WHITE);
    reset(fb);

    aaellipseColor(fb, 100, 100, 50, 25, Colors::RED);
    reset(fb);

    filledEllipseColor(fb, 100, 100, 50, 25, Colors::GREEN);
    reset(fb);

    pieColor(fb, 100, 100, 50, 0, 90, Colors::BLUE);
    reset(fb);

    filledPieColor(fb, 100, 100, 50, 0, 90, Colors::CYAN);
    reset(fb);

    trigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::MAGENTA);
    reset(fb);

    aatrigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::YELLOW);
    reset(fb);

    filledTrigonColor(fb, 100, 100, 100, 200, 200, 100, Colors::WHITE);
    reset(fb);

    const short x[] = {100, 200, 300}, y[] = {100, 200, 100};
    polygonColor(fb, x, y, 3, Colors::RED);
    reset(fb);

    aapolygonColor(fb, x, y, 3, Colors::GREEN);
    reset(fb);

    filledPolygonColor(fb, x, y, 3, Colors::BLUE);
    reset(fb);

    bezierColor(fb, x, y, 3, 10, Colors::CYAN);
    reset(fb);

    thickLineColor(fb, 100, 100, 200, 200, 10, Colors::MAGENTA);
    reset(fb);

    return EXIT_SUCCESS;
}
