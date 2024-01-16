// Modified from ferzkopp's SDL2_gfxPrimitives library
#pragma once

#include <string>
#include <drawing.h>

constexpr int AA_BITS = 8, ELLIPSE_LIMIT = 4;

class Helpers
{
public:
    static void warn(const std::string &message);
    static void pixel(Framebuffer renderer, short x, short y, unsigned int color);
    static void pixelRGBAWeight(const Framebuffer &renderer, short x, short y, unsigned char r, unsigned char g,
                                unsigned char b, unsigned char a, unsigned int weight);
    static void aalineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char r,
                           unsigned char g, unsigned char b, unsigned char a, bool draw_endpoint);
    static void drawQuadrants(Framebuffer renderer, short x, short y, short dx, short dy, int f, unsigned int color);
    static void ellipseRGBA(Framebuffer renderer, short x, short y, short rx, short ry, unsigned char r,
                            unsigned char g, unsigned char b, unsigned char a, int f);
    static void
    pieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
            unsigned char g, unsigned char b, unsigned char a, unsigned char filled);
    static void polygon(Framebuffer renderer, const short* vx, const short* vy, int n, unsigned int color);
    static auto compareInt(const void* a, const void* b) -> int;
    static auto evaluateBezier(double* data, int nData, double t) -> double;
};

/* Pixel */

void pixelColor(const Framebuffer &renderer, short x, short y, unsigned int color);
void pixelRGBA(Framebuffer renderer, short x, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Horizontal line */

void hlineColor(const Framebuffer &renderer, short x1, short x2, short y, unsigned int color);
void hlineRGBA(Framebuffer renderer, short x1, short x2, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Vertical line */

void vlineColor(const Framebuffer &renderer, short x, short y1, short y2, unsigned int color);
void vlineRGBA(Framebuffer renderer, short x, short y1, short y2, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Rectangle */

void rectangleColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color);
void rectangleRGBA(Framebuffer renderer, short x1, short y1,
                   short x2, short y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/* Rounded-Corner Rectangle */

void roundedRectangleColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad,
                           unsigned int color);
void
roundedRectangleRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad, unsigned char r,
                     unsigned char g, unsigned char b, unsigned char a);

/* Filled rectangle (Box) */

void boxColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color);
void boxRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
             unsigned char b, unsigned char a);

/* Rounded-Corner Filled rectangle (Box) */

void
roundedBoxColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad, unsigned int color);
void roundedBoxRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, short rad, unsigned char r,
                    unsigned char g, unsigned char b, unsigned char a);

/* Line */

void lineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color);
void lineRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
              unsigned char b, unsigned char a);

/* AA Line */

void aalineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color);
void aalineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a);

/* Circle */

void circleColor(const Framebuffer &renderer, short x, short y, short rad, unsigned int color);
void
circleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g, unsigned char b,
           unsigned char a);

/* Arc */

void arcColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned int color);
void arcRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a);

/* AA Circle */

void aacircleColor(const Framebuffer &renderer, short x, short y, short rad, unsigned int color);
void aacircleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a);

/* Ellipse */

void ellipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color);
void ellipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a);

/* Filled Circle */

void filledCircleColor(const Framebuffer &renderer, short x, short y, short r, unsigned int color);
void filledCircleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a);

/* AA Ellipse */

void aaellipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color);
void aaellipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a);

/* Filled Ellipse */

void filledEllipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color);
void
filledEllipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a);

/* Pie */

void pieColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned int color);
void pieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a);

/* Filled Pie */

void
filledPieColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned int color);
void filledPieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
                   unsigned char g, unsigned char b, unsigned char a);

/* Trigon */

void trigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                 unsigned int color);
void
trigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3, unsigned char r,
           unsigned char g, unsigned char b, unsigned char a);

/* AA-Trigon */

void aatrigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                   unsigned int color);
void
aatrigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a);

/* Filled Trigon */

void filledTrigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                       unsigned int color);
void filledTrigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                      unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/* Polygon */

void polygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color);
void polygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a);

/* AA-Polygon */

void aapolygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color);
void
aapolygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r, unsigned char g,
              unsigned char b, unsigned char a);

/* Filled Polygon */

void filledPolygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color);
void filledPolygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r,
                       unsigned char g, unsigned char b, unsigned char a);

/* Bezier */

void bezierColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, int s, unsigned int color);
void bezierRGBA(Framebuffer renderer, const short* vx, const short* vy, int n, int s, unsigned char r,
                unsigned char g, unsigned char b, unsigned char a);

/* Thick Line */

void thickLineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char width,
                    unsigned int color);
void
thickLineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char width, unsigned char r,
              unsigned char g, unsigned char b, unsigned char a);
