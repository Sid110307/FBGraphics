#include <array>

#include "drawing.h"
#include "utils.h"

constexpr int WIDTH = 1920, HEIGHT = 1080;

static constexpr int LUT = 4096;
static std::array<unsigned int, LUT> pal;

void mandelbrot(Framebuffer& fb, const double centerX, const double centerY, const double zoom, const int maxIterations)
{
    const double viewW = 3.5 / zoom, viewH = 2.0 / zoom;
    const double x0 = centerX - viewW * 0.5;
    const double y0 = centerY - viewH * 0.5;
    const double dx = viewW / static_cast<double>(WIDTH);
    const double dy = viewH / static_cast<double>(HEIGHT);

    for (int y = 0; y < HEIGHT; ++y)
    {
        const double cb = y0 + y * dy;
        double ca = x0;

        for (int x = 0; x < WIDTH; ++x, ca += dx)
        {
            double a = ca, b = cb;
            int n = 0;

            double a2 = a * a, b2 = b * b;
            while (n < maxIterations && (a2 + b2) <= 16.0)
            {
                b = 2.0 * a * b + cb;
                a = a2 - b2 + ca;
                a2 = a * a;
                b2 = b * b;
                ++n;
            }

            unsigned int col;
            if (n == maxIterations) col = Colors::BLACK;
            else
            {
                constexpr double invLog2 = 1.4426950408889634;
                const double r2 = a2 + b2;
                const double mu = n + 1 - std::log(0.5 * std::log(r2)) * invLog2;
                double t = mu / static_cast<double>(maxIterations);
                t = std::pow(t, 0.35);

                int idx = static_cast<int>(t * (LUT - 1));
                if (idx < 0) idx = 0;
                if (idx >= LUT) idx = LUT - 1;
                col = pal[idx];
            }

            Pixel(fb, static_cast<float>(x), static_cast<float>(y)).draw(col);
        }
    }
}

int main()
{
    Framebuffer fb(WIDTH, HEIGHT, 4);

    double zoom = 1.0;
    int maxIterations = 200;

    for (int i = 0; i < LUT; ++i)
    {
        const double t = static_cast<double>(i) / (LUT - 1);
        const double r = 0.5 + 0.5 * std::cos(6.28318 * (t + 0.00));
        const double g = 0.5 + 0.5 * std::cos(6.28318 * (t + 0.33));
        const double b = 0.5 + 0.5 * std::cos(6.28318 * (t + 0.67));

        pal[i] = Colors::RGBA(static_cast<unsigned int>(255 * r), static_cast<unsigned int>(255 * g),
                              static_cast<unsigned int>(255 * b), 255);
    }

    while (true)
    {
        constexpr double centerX = -0.743643887037151, centerY = 0.131825904205330;
        const char c = getChar();

        if (c == 'q') break;
        if (c == 's')
        {
            zoom *= 1.85;
            maxIterations = static_cast<int>(maxIterations * 1.35) + 10;
        }

        mandelbrot(fb, centerX, centerY, zoom, maxIterations);
        fb.present(0, 0);
    }
    return 0;
}
