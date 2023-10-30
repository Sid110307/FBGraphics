#pragma once

#include "framebuffer.h"

struct Point
{
    float x, y;
};

class Drawable
{
public:
    Drawable(Framebuffer &fb, float x, float y) : fb(fb), x(x), y(y), color(0x00000000) {}

    virtual void draw(unsigned int color) = 0;
    void setPos(float _x, float _y);
    [[nodiscard]] Point getPos() const;

protected:
    Framebuffer &fb;
    float x, y;
    unsigned int color;
};

class Pixel : public Drawable
{
public:
    Pixel(Framebuffer &fb, float x, float y) : Drawable(fb, x, y) {}
    void draw(unsigned int color) override;
    void update(float newX, float newY);
};

class Line : public Drawable
{
public:
    Line(Framebuffer &fb, float x1, float y1, float x2, float y2)
            : Drawable(fb, (x1 + x2) / 2, (y1 + y2) / 2), x1(x1), y1(y1), x2(x2), y2(y2) {}

    void draw(unsigned int color) override;
    void update(float newX1, float newY1, float newX2, float newY2);

private:
    float x1, y1, x2, y2;
};

class Rectangle : public Drawable
{
public:
    Rectangle(Framebuffer &fb, float x, float y, float width, float height, bool filled = false)
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
    Circle(Framebuffer &fb, float x, float y, float radius, bool filled = false)
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
    Triangle(Framebuffer &fb, float x1, float y1, float x2, float y2, float x3, float y3, bool filled = false)
            : Drawable(fb, (x1 + x2 + x3) / 3, (y1 + y2 + y3) / 3), x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3),
              filled(filled) {}

    void draw(unsigned int color) override;
    void update(float newX1, float newY1, float newX2, float newY2, float newX3, float newY3);

private:
    float x1, y1, x2, y2, x3, y3;
    bool filled;
};
