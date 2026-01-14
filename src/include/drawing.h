#pragma once

#include "framebuffer.h"
#include "utils.h"

class Drawable
{
public:
    Drawable(Framebuffer& fb, const float x, const float y) : _fb(fb), _x(x), _y(y), _color(Colors::BLACK) {}
    virtual ~Drawable() = default;

    virtual void draw(unsigned int color);
    void setPos(float x, float y);
    [[nodiscard]] std::pair<float, float> getPos() const;

protected:
    Framebuffer& _fb;
    float _x, _y;
    unsigned int _color;
};

class Pixel : public Drawable
{
public:
    Pixel(Framebuffer& fb, const float x, const float y) : Drawable(fb, x, y) {}

    void draw(unsigned int color) override;
    void update(float newX, float newY);
};

class Line : public Drawable
{
public:
    Line(Framebuffer& fb, const float x1, const float y1, const float x2, const float y2)
        : Drawable(fb, (x1 + x2) / 2, (y1 + y2) / 2), x1(x1), y1(y1), x2(x2), y2(y2) {}

    void draw(unsigned int color) override;
    void update(float newX1, float newY1, float newX2, float newY2);

private:
    float x1, y1, x2, y2;
};

class Rect : public Drawable
{
public:
    Rect(Framebuffer& fb, const float x, const float y, const float width, const float height,
         const bool filled)
        : Drawable(fb, x + width / 2, y + height / 2), width(width), height(height), filled(filled) {}

    void draw(unsigned int color) override;
    void update(float newX, float newY, float newWidth, float newHeight);

private:
    float width, height;
    bool filled;
};

class Circle : public Drawable
{
public:
    Circle(Framebuffer& fb, const float x, const float y, const float radius, const bool filled)
        : Drawable(fb, x, y), radius(radius), filled(filled) {}

    void draw(unsigned int color) override;
    void update(float newRadius);

private:
    float radius;
    bool filled;
};

class Triangle : public Drawable
{
public:
    Triangle(Framebuffer& fb, const float x1, const float y1, const float x2, const float y2, const float x3,
             const float y3, const bool filled)
        : Drawable(fb, (x1 + x2 + x3) / 3, (y1 + y2 + y3) / 3), x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3),
          filled(filled) {}

    void draw(unsigned int color) override;
    void update(float newX1, float newY1, float newX2, float newY2, float newX3, float newY3);

private:
    float x1, y1, x2, y2, x3, y3;
    bool filled;
};
