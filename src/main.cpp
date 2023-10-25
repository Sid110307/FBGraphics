#include <thread>
#include "include/drawing.h"

constexpr unsigned int WIDTH = 1920, HEIGHT = 1080;

int main()
{
    Framebuffer framebuffer("/dev/fb0", WIDTH, HEIGHT, 4);

    float x = 100, y = 100, radius = 50;

    Circle circle(framebuffer, x, y, radius);
    circle.draw(0x00FF00FF);

    float velocity = 0;
    float acceleration = 0.1;

    while (true)
    {
        circle.setPos(x, y);

        if (x + radius >= WIDTH || x - radius <= 0)
        {
            velocity = -velocity;
            acceleration = -acceleration;
        }

        velocity += acceleration;
        x += velocity;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return EXIT_SUCCESS;
}
