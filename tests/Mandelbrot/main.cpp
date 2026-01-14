#include "drawing.h"
#include "utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

double map(const double value, const double start1, const double stop1, const double start2, const double stop2)
{
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

void mandelbrot(Framebuffer& fb)
{
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
        {
            double a = map(x, 0, WIDTH, -2.5, 1), b = map(y, 0, HEIGHT, -1, 1);
            const double ca = a, cb = b;

            constexpr int maxIterations = 100;
            int n = 0;

            while (n < maxIterations)
            {
                const double aa = a * a - b * b, bb = 2 * a * b;

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
    Framebuffer fb(WIDTH, HEIGHT, 4);

    while (true)
    {
        if (const char c = getChar(); c == 'q') break;

        mandelbrot(fb);
        fb.present(0, 0);
    }
    return 0;
}
