// Modified from ferzkopp's SDL2_gfxPrimitives
// https://github.com/ferzkopp/SDL2_gfx

#pragma once

#include "drawing.h"

constexpr int AA_BITS = 8, ELLIPSE_LIMIT = 4;
constexpr float M_PI = 3.14159265358979323846f;

class Helpers
{
public:
    static void warn(const std::string& message);
    static void pixel(Framebuffer& renderer, short x, short y, unsigned int color);
    static void pixelRGBAWeight(Framebuffer& renderer, short x, short y, unsigned char r, unsigned char g,
                                unsigned char b, unsigned char a, unsigned int weight);
    static void aalineRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char r,
                           unsigned char g, unsigned char b, unsigned char a, bool drawEndpoint);
    static void drawQuadrants(Framebuffer& renderer, short x, short y, short dx, short dy, int f, unsigned int color);
    static void ellipseRGBA(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned char r,
                            unsigned char g, unsigned char b, unsigned char a, int f);
    static void pieRGBA(Framebuffer& renderer, short x, short y, short rad, short start, short end,
                        unsigned char r,
                        unsigned char g, unsigned char b, unsigned char a, unsigned char filled);
    static void polygon(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned int color);
    static auto compareInt(const void* a, const void* b) -> int;
    static auto evaluateBezier(const double* data, int nData, double t) -> double;
};

/* Pixel */

void pixelColor(Framebuffer& renderer, short x, short y, unsigned int color);
void pixelRGBA(Framebuffer& renderer, short x, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Horizontal line */

void hlineColor(Framebuffer& renderer, short x1, short x2, short y, unsigned int color);
void hlineRGBA(Framebuffer& renderer, short x1, short x2, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Vertical line */

void vlineColor(Framebuffer& renderer, short x, short y1, short y2, unsigned int color);
void vlineRGBA(Framebuffer& renderer, short x, short y1, short y2, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a);

/* Rectangle */

void rectangleColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned int color);
void rectangleRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a);

/* Rounded-Corner Rectangle */

void roundedRectangleColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad,
                           unsigned int color);
void roundedRectangleRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad,
                          unsigned char r,
                          unsigned char g, unsigned char b, unsigned char a);

/* Filled rectangle (Box) */

void boxColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned int color);
void boxRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
             unsigned char b, unsigned char a);

/* Rounded-Corner Filled rectangle (Box) */

void roundedBoxColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad,
                     unsigned int color);
void roundedBoxRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad, unsigned char r,
                    unsigned char g, unsigned char b, unsigned char a);

/* Line */

void lineColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned int color);
void lineRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
              unsigned char b, unsigned char a);

/* AA Line */

void aalineColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned int color);
void aalineRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a);

/* Circle */

void circleColor(Framebuffer& renderer, short x, short y, short rad, unsigned int color);
void circleRGBA(Framebuffer& renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                unsigned char b,
                unsigned char a);

/* Arc */

void arcColor(Framebuffer& renderer, short x, short y, short rad, short start, short end, unsigned int color);
void arcRGBA(Framebuffer& renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a);

/* AA Circle */

void aacircleColor(Framebuffer& renderer, short x, short y, short rad, unsigned int color);
void aacircleRGBA(Framebuffer& renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a);

/* Ellipse */

void ellipseColor(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned int color);
void ellipseRGBA(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a);

/* Filled Circle */

void filledCircleColor(Framebuffer& renderer, short x, short y, short r, unsigned int color);
void filledCircleRGBA(Framebuffer& renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a);

/* AA Ellipse */

void aaellipseColor(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned int color);
void aaellipseRGBA(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a);

/* Filled Ellipse */

void filledEllipseColor(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned int color);
void filledEllipseRGBA(Framebuffer& renderer, short x, short y, short rx, short ry, unsigned char r,
                       unsigned char g,
                       unsigned char b, unsigned char a);

/* Pie */

void pieColor(Framebuffer& renderer, short x, short y, short rad, short start, short end, unsigned int color);
void pieRGBA(Framebuffer& renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a);

/* Filled Pie */

void filledPieColor(Framebuffer& renderer, short x, short y, short rad, short start, short end,
                    unsigned int color);
void filledPieRGBA(Framebuffer& renderer, short x, short y, short rad, short start, short end, unsigned char r,
                   unsigned char g, unsigned char b, unsigned char a);

/* Trigon */

void trigonColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                 unsigned int color);
void trigonRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                unsigned char r,
                unsigned char g, unsigned char b, unsigned char a);

/* AA-Trigon */

void aatrigonColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                   unsigned int color);
void aatrigonRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                  unsigned char r,
                  unsigned char g, unsigned char b, unsigned char a);

/* Filled Trigon */

void filledTrigonColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                       unsigned int color);
void filledTrigonRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                      unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/* Polygon */

void polygonColor(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned int color);
void polygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a);

/* AA-Polygon */

void aapolygonColor(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned int color);
void aapolygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned char r,
                   unsigned char g,
                   unsigned char b, unsigned char a);

/* Filled Polygon */

void filledPolygonColor(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned int color);
void filledPolygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, int n, unsigned char r,
                       unsigned char g, unsigned char b, unsigned char a);

/* Bezier */

void bezierColor(Framebuffer& renderer, const short* vx, const short* vy, int n, int s, unsigned int color);
void bezierRGBA(Framebuffer& renderer, const short* vx, const short* vy, int n, int s, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a);

/* Thick Line */

void thickLineColor(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char width,
                    unsigned int color);
void thickLineRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, unsigned char width,
                   unsigned char r,
                   unsigned char g, unsigned char b, unsigned char a);
