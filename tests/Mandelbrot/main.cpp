#include "drawing.h"
#include "utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

double map(double value, double start1, double stop1, double start2, double stop2)
{
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

void mandelbrot(Framebuffer &fb)
{
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
        {
            double a = map(x, 0, WIDTH, -2.5, 1), b = map(y, 0, HEIGHT, -1, 1);
            double ca = a, cb = b;

            int n = 0, maxIterations = 100;
            while (n < maxIterations)
            {
                double aa = a * a - b * b, bb = 2 * a * b;

                a = aa + ca;
                b = bb + cb;

                if (a * a + b * b > 16) break;
                n++;
            }

            Pixel(fb, static_cast<float>(x), static_cast<float>(y)).draw(
                n == maxIterations ? Colors::WHITE : Colors::BLUE);
        }

}

int main()
{
    Framebuffer fb("/dev/fb0", WIDTH, HEIGHT, 4);
    mandelbrot(fb);

    while (true)
    {
        char c = getChar();
        if (c == 'q') return EXIT_SUCCESS;
    }
}
