#include "include/drawing.h"

void Drawable::setPos(float _x, float _y)
{
    draw(0x00000000);

    this->x = _x;
    this->y = _y;

    draw(color);
}

Point Drawable::getPos() const { return {x, y}; }
void Pixel::draw(unsigned int color) { fb.drawPixel(x, y, color); }

void Pixel::update(float newX, float newY)
{
    draw(0x00000000);

    x = newX;
    y = newY;

    draw(color);
}

void Line::draw(unsigned int color)
{
    if (color != 0x00000000) this->color = color;

    int steps = static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
    float dx = (x2 - x1) / static_cast<float>(steps), dy = (y2 - y1) / static_cast<float>(steps);

    for (int i = 0; i <= steps; ++i)
        fb.drawPixel(x1 + static_cast<float>(i) * dx, y1 + static_cast<float>(i) * dy, color);
}

void Line::update(float newX1, float newY1, float newX2, float newY2)
{
    draw(0x00000000);

    x1 = newX1;
    y1 = newY1;
    x2 = newX2;
    y2 = newY2;

    x = (newX1 + newX2) / 2;
    y = (newY1 + newY2) / 2;
    draw(color);
}

void Rectangle::draw(unsigned int color)
{
    if (color != 0x00000000) this->color = color;

    if (filled)
        for (int i = 0; i < static_cast<int>(height); ++i)
            Line(fb, x - width / 2, y - height / 2 + static_cast<float>(i), x + width / 2,
                 y - height / 2 + static_cast<float>(i)).draw(color);
    else
    {
        Line(fb, x - width / 2, y - height / 2, x + width / 2, y - height / 2).draw(color);
        Line(fb, x - width / 2, y + height / 2, x + width / 2, y + height / 2).draw(color);
        Line(fb, x - width / 2, y - height / 2, x - width / 2, y + height / 2).draw(color);
        Line(fb, x + width / 2, y - height / 2, x + width / 2, y + height / 2).draw(color);
    }
}

void Rectangle::update(float newX, float newY, float newWidth, float newHeight)
{
    draw(0x00000000);

    x = newX + newWidth / 2;
    y = newY + newHeight / 2;
    width = newWidth;
    height = newHeight;

    draw(color);
}

void Circle::draw(unsigned int color)
{
    if (color != 0x00000000) this->color = color;

    if (filled)
    {
        int r = static_cast<int>(radius);

        for (int y = -r; y <= r; ++y)
            for (int x = -r; x <= r; ++x)
                if (x * x + y * y <= r * r)
                    fb.drawPixel(this->x + static_cast<float>(x), this->y + static_cast<float>(y), color);
    } else
    {
        int r = static_cast<int>(radius);
        int x = 0, y = r;
        int d = 3 - 2 * r;

        while (y >= x)
        {
            fb.drawPixel(this->x + static_cast<float>(x), this->y + static_cast<float>(y), color);
            fb.drawPixel(this->x + static_cast<float>(y), this->y + static_cast<float>(x), color);
            fb.drawPixel(this->x - static_cast<float>(x), this->y + static_cast<float>(y), color);
            fb.drawPixel(this->x - static_cast<float>(y), this->y + static_cast<float>(x), color);
            fb.drawPixel(this->x + static_cast<float>(x), this->y - static_cast<float>(y), color);
            fb.drawPixel(this->x + static_cast<float>(y), this->y - static_cast<float>(x), color);
            fb.drawPixel(this->x - static_cast<float>(x), this->y - static_cast<float>(y), color);
            fb.drawPixel(this->x - static_cast<float>(y), this->y - static_cast<float>(x), color);

            if (d < 0) d += 4 * x + 6;
            else
            {
                d += 4 * (x - y) + 10;
                --y;
            }

            ++x;
        }
    }
}

void Circle::update(float newRadius)
{
    draw(0x00000000);

    radius = newRadius;
    draw(color);
}
