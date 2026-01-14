#include "include/drawing.h"

void Drawable::draw(const unsigned int color)
{
    if (color != Colors::BLACK) this->_color = color;
    _fb.drawPixel(_x, _y, color);
}

void Drawable::setPos(const float x, const float y)
{
    this->_x = x;
    this->_y = y;
}

std::pair<float, float> Drawable::getPos() const { return {_x, _y}; }

void Pixel::draw(const unsigned int color)
{
    if (color != Colors::BLACK) this->_color = color;
    _fb.drawPixel(_x, _y, color);
}

void Pixel::update(const float newX, const float newY)
{
    _x = newX;
    _y = newY;
}

void Line::draw(const unsigned int color)
{
    if (color != Colors::BLACK) this->_color = color;

    const int steps = static_cast<int>(hypot(x2 - x1, y2 - y1));
    if (steps == 0)
    {
        _fb.drawPixel(x1, y1, color);
        return;
    }

    const float dx = (x2 - x1) / static_cast<float>(steps), dy = (y2 - y1) / static_cast<float>(steps);
    for (int i = 0; i <= steps; ++i)
        _fb.drawPixel(x1 + static_cast<float>(i) * dx, y1 + static_cast<float>(i) * dy,
                      color);
}

void Line::update(const float newX1, const float newY1, const float newX2, const float newY2)
{
    x1 = newX1;
    y1 = newY1;
    x2 = newX2;
    y2 = newY2;

    _x = (newX1 + newX2) / 2;
    _y = (newY1 + newY2) / 2;
}

void Rect::draw(const unsigned int color)
{
    if (color != Colors::BLACK) this->_color = color;

    if (filled)
        for (int i = 0; i < static_cast<int>(height); ++i)
            Line(_fb, _x - width / 2, _y - height / 2 + static_cast<float>(i), _x + width / 2,
                 _y - height / 2 + static_cast<float>(i)).draw(color);
    else
    {
        Line(_fb, _x - width / 2, _y - height / 2, _x + width / 2, _y - height / 2).draw(color);
        Line(_fb, _x - width / 2, _y + height / 2, _x + width / 2, _y + height / 2).draw(color);
        Line(_fb, _x - width / 2, _y - height / 2, _x - width / 2, _y + height / 2).draw(color);
        Line(_fb, _x + width / 2, _y - height / 2, _x + width / 2, _y + height / 2).draw(color);
    }
}

void Rect::update(const float newX, const float newY, const float newWidth, const float newHeight)
{
    _x = newX + newWidth / 2;
    _y = newY + newHeight / 2;
    width = newWidth;
    height = newHeight;
}

void Circle::draw(const unsigned int color)
{
    if (color != Colors::BLACK) this->_color = color;

    if (filled)
    {
        const int r = static_cast<int>(radius);

        for (int y = -r; y <= r; ++y)
            for (int x = -r; x <= r; ++x)
                if (x * x + y * y <= r * r)
                    _fb.drawPixel(this->_x + static_cast<float>(x),
                                  this->_y + static_cast<float>(y), color);
    }
    else
    {
        int r = static_cast<int>(radius), x = 0, y = r, d = 3 - 2 * r;

        while (y >= x)
        {
            _fb.drawPixel(this->_x + static_cast<float>(x), this->_y + static_cast<float>(y), color);
            _fb.drawPixel(this->_x + static_cast<float>(y), this->_y + static_cast<float>(x), color);
            _fb.drawPixel(this->_x - static_cast<float>(x), this->_y + static_cast<float>(y), color);
            _fb.drawPixel(this->_x - static_cast<float>(y), this->_y + static_cast<float>(x), color);
            _fb.drawPixel(this->_x + static_cast<float>(x), this->_y - static_cast<float>(y), color);
            _fb.drawPixel(this->_x + static_cast<float>(y), this->_y - static_cast<float>(x), color);
            _fb.drawPixel(this->_x - static_cast<float>(x), this->_y - static_cast<float>(y), color);
            _fb.drawPixel(this->_x - static_cast<float>(y), this->_y - static_cast<float>(x), color);

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

void Circle::update(const float newRadius) { radius = newRadius; }

void Triangle::draw(const unsigned int color)
{
    auto isInside = [&](const float x, const float y) -> bool
    {
        const float A = 1.0f / 2.0f * (-y2 * x3 + y1 * (-x2 + x3) + x1 * (y2 - y3) + x2 * y3);
        const float sign = A < 0.0f ? -1.0f : 1.0f;
        const float s = (y1 * x3 - x1 * y3 + (y3 - y1) * x + (x1 - x3) * y) * sign;
        const float t = (x1 * y2 - y1 * x2 + (y1 - y2) * x + (x2 - x1) * y) * sign;

        return s > 0.0f && t > 0.0f && s + t < 2.0f * A * sign;
    };

    if (color != Colors::BLACK) this->_color = color;
    if (filled)
    {
        const float minX = min(x1, min(x2, x3)), maxX = max(x1, max(x2, x3));
        const float minY = min(y1, min(y2, y3)), maxY = max(y1, max(y2, y3));

        for (int y = static_cast<int>(minY); y <= static_cast<int>(maxY); ++y)
            for (int x = static_cast<int>(minX); x <= static_cast<int>(maxX); ++x)
                if (isInside(static_cast<float>(x), static_cast<float>(y)))
                    _fb.drawPixel(
                        static_cast<float>(x), static_cast<float>(y), color);
    }
    else
    {
        Line(_fb, x1, y1, x2, y2).draw(color);
        Line(_fb, x2, y2, x3, y3).draw(color);
        Line(_fb, x3, y3, x1, y1).draw(color);
    }
}

void Triangle::update(const float newX1, const float newY1, const float newX2, const float newY2, const float newX3,
                      const float newY3)
{
    x1 = newX1;
    y1 = newY1;
    x2 = newX2;
    y2 = newY2;
    x3 = newX3;
    y3 = newY3;

    _x = (newX1 + newX2 + newX3) / 3;
    _y = (newY1 + newY2 + newY3) / 3;
}
