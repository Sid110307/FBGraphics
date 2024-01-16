#include "include/drawing.h"

void Drawable::draw(unsigned int _color, bool interpolate) { fb.drawPixel(x, y, _color, interpolate); }

void Drawable::setPos(float _x, float _y)
{
    draw(0x00000000);

    this->x = _x;
    this->y = _y;

    draw(color);
}

std::pair<float, float> Drawable::getPos() const { return {x, y}; }
void Pixel::draw(unsigned int color, bool interpolate) { fb.drawPixel(x, y, color, interpolate); }

void Pixel::update(float newX, float newY)
{
    draw(0x00000000);

    x = newX;
    y = newY;

    draw(color);
}

void Line::draw(unsigned int color, bool interpolate)
{
    if (color != 0x00000000) this->color = color;

    int steps = static_cast<int>(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
    float dx = (x2 - x1) / static_cast<float>(steps), dy = (y2 - y1) / static_cast<float>(steps);

    for (int i = 0; i <= steps; ++i)
        fb.drawPixel(x1 + static_cast<float>(i) * dx, y1 + static_cast<float>(i) * dy, color, interpolate);
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

void Rectangle::draw(unsigned int color, bool interpolate)
{
    if (color != 0x00000000) this->color = color;

    if (filled)
        for (int i = 0; i < static_cast<int>(height); ++i)
            Line(fb, x - width / 2, y - height / 2 + static_cast<float>(i), x + width / 2,
                 y - height / 2 + static_cast<float>(i)).draw(color, interpolate);
    else
    {
        Line(fb, x - width / 2, y - height / 2, x + width / 2, y - height / 2).draw(color, interpolate);
        Line(fb, x - width / 2, y + height / 2, x + width / 2, y + height / 2).draw(color, interpolate);
        Line(fb, x - width / 2, y - height / 2, x - width / 2, y + height / 2).draw(color, interpolate);
        Line(fb, x + width / 2, y - height / 2, x + width / 2, y + height / 2).draw(color, interpolate);
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

void Circle::draw(unsigned int color, bool interpolate)
{
    if (color != 0x00000000) this->color = color;

    if (filled)
    {
        int r = static_cast<int>(radius);

        for (int y = -r; y <= r; ++y)
            for (int x = -r; x <= r; ++x)
                if (x * x + y * y <= r * r)
                    fb.drawPixel(this->x + static_cast<float>(x), this->y + static_cast<float>(y), color, interpolate);
    } else
    {
        int r = static_cast<int>(radius), x = 0, y = r, d = 3 - 2 * r;

        while (y >= x)
        {
            fb.drawPixel(this->x + static_cast<float>(x), this->y + static_cast<float>(y), color, interpolate);
            fb.drawPixel(this->x + static_cast<float>(y), this->y + static_cast<float>(x), color, interpolate);
            fb.drawPixel(this->x - static_cast<float>(x), this->y + static_cast<float>(y), color, interpolate);
            fb.drawPixel(this->x - static_cast<float>(y), this->y + static_cast<float>(x), color, interpolate);
            fb.drawPixel(this->x + static_cast<float>(x), this->y - static_cast<float>(y), color, interpolate);
            fb.drawPixel(this->x + static_cast<float>(y), this->y - static_cast<float>(x), color, interpolate);
            fb.drawPixel(this->x - static_cast<float>(x), this->y - static_cast<float>(y), color, interpolate);
            fb.drawPixel(this->x - static_cast<float>(y), this->y - static_cast<float>(x), color, interpolate);

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

void Triangle::draw(unsigned int color, bool interpolate)
{
    auto isInside = [&](float x, float y) -> bool
    {
        float A = 1.0f / 2.0f * (-y2 * x3 + y1 * (-x2 + x3) + x1 * (y2 - y3) + x2 * y3);
        float sign = A < 0.0f ? -1.0f : 1.0f;
        float s = (y1 * x3 - x1 * y3 + (y3 - y1) * x + (x1 - x3) * y) * sign;
        float t = (x1 * y2 - y1 * x2 + (y1 - y2) * x + (x2 - x1) * y) * sign;

        return s > 0.0f && t > 0.0f && (s + t) < 2.0f * A * sign;
    };

    if (color != 0x00000000) this->color = color;

    if (filled)
    {
        float minX = std::min(x1, std::min(x2, x3)), maxX = std::max(x1, std::max(x2, x3));
        float minY = std::min(y1, std::min(y2, y3)), maxY = std::max(y1, std::max(y2, y3));

        for (int y = static_cast<int>(minY); y <= static_cast<int>(maxY); ++y)
            for (int x = static_cast<int>(minX); x <= static_cast<int>(maxX); ++x)
                if (isInside(static_cast<float>(x), static_cast<float>(y)))
                    fb.drawPixel(static_cast<float>(x), static_cast<float>(y), color, interpolate);
    } else
    {
        Line(fb, x1, y1, x2, y2).draw(color, interpolate);
        Line(fb, x2, y2, x3, y3).draw(color, interpolate);
        Line(fb, x3, y3, x1, y1).draw(color, interpolate);
    }
}

void Triangle::update(float newX1, float newY1, float newX2, float newY2, float newX3, float newY3)
{
    draw(0x00000000);

    x1 = newX1;
    y1 = newY1;
    x2 = newX2;
    y2 = newY2;
    x3 = newX3;
    y3 = newY3;

    x = (newX1 + newX2 + newX3) / 3;
    y = (newY1 + newY2 + newY3) / 3;
    draw(color);
}
