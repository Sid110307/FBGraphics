#include <complex>
#include "include/drawing.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

int mandelbrot(const std::complex<double>& c, int maxIterations)
{
    std::complex<double> z = 0;
    int n = 0;

    while (std::abs(z) <= 2.0 && n < maxIterations)
    {
        z = z * z + c;
        n++;
    }

    return n;
}

int main()
{
    Framebuffer framebuffer("/dev/fb0", WIDTH, HEIGHT, 4);

    const double realMin = -2.0, realMax = 1.0, imagMin = -1.5, imagMax = 1.5;
    const int maxIterations = 1000;
    const double realStep = (realMax - realMin) / WIDTH, imagStep = (imagMax - imagMin) / HEIGHT;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            std::complex<double> c(realMin + x * realStep, imagMin + y * imagStep);
            int value = mandelbrot(c, maxIterations);
            unsigned int color = (value == maxIterations) ? 0x00FF00FF : 0x00000000;

            framebuffer.drawPixel(x, y, color);
        }
    }

    return EXIT_SUCCESS;
}

