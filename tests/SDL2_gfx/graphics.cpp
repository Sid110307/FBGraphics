#include "graphics.h"

void Helpers::warn(const std::string &message) { std::cerr << "Warning: " << message << std::endl; }
void Helpers::pixel(Framebuffer renderer, short x, short y, unsigned int color) { Pixel(renderer, x, y).draw(color); }

void Helpers::pixelRGBAWeight(const Framebuffer &renderer, short x, short y, unsigned char r, unsigned char g,
                              unsigned char b, unsigned char a, unsigned int weight)
{
    unsigned int ax = a;
    ax = (ax * weight >> 8);
    a = ax > 255 ? 255 : static_cast<unsigned char>(ax & 0x000000ff);

    pixelRGBA(renderer, x, y, r, g, b, a);
}

void Helpers::aalineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char r,
                         unsigned char g, unsigned char b, unsigned char a, bool draw_endpoint)
{
    int xx0, yy0, xx1, yy1, dx, dy, tmp, xDir, y0p1, x0pxDir;
    unsigned int intShift, errAcc, errAdj, errAccTmp, weight;

    xx0 = x1;
    yy0 = y1;
    xx1 = x2;
    yy1 = y2;

    if (yy0 > yy1)
    {
        tmp = yy0;
        yy0 = yy1;
        yy1 = tmp;

        tmp = xx0;
        xx0 = xx1;
        xx1 = tmp;
    }

    dx = xx1 - xx0;
    dy = yy1 - yy0;

    if (dx >= 0) xDir = 1;
    else
    {
        xDir = -1;
        dx = (-dx);
    }

    if (dx == 0)
        draw_endpoint ? vlineRGBA(renderer, x1, y1, y2, r, g, b, a)
                      : dy > 0 ? vlineRGBA(renderer, x1, static_cast<short>(yy0), static_cast<short>(yy0 + dy),
                                           r, g, b, a) : pixelRGBA(renderer, x1, y1, r, g, b, a);
    else if (dy == 0)
        draw_endpoint ? hlineRGBA(renderer, x1, x2, y1, r, g, b, a)
                      : dx > 0 ? hlineRGBA(renderer, static_cast<short>(xx0),
                                           static_cast<short>(xx0 + (xDir * dx)), y1, r, g, b, a) : pixelRGBA(
            renderer, x1, y1, r, g, b, a);
    else if (dx == dy && draw_endpoint) lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);

    errAcc = 0;
    intShift = 32 - AA_BITS;
    pixelRGBA(renderer, x1, y1, r, g, b, a);

    if (dy > dx)
    {
        errAdj = ((dx << 16) / dy) << 16;
        x0pxDir = xx0 + xDir;

        while (--dy)
        {
            errAccTmp = errAcc;
            errAcc += errAdj;

            if (errAcc <= errAccTmp)
            {
                xx0 = x0pxDir;
                x0pxDir += xDir;
            }

            yy0++;
            weight = (errAcc >> intShift) & 255;
            Helpers::pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(yy0), r, g, b, a,
                                     255 - weight);
            Helpers::pixelRGBAWeight(renderer, static_cast<short>(x0pxDir), static_cast<short>(yy0), r, g, b, a,
                                     weight);
        }
    } else
    {
        errAdj = ((dy << 16) / dx) << 16;
        y0p1 = yy0 + 1;

        while (--dx)
        {
            errAccTmp = errAcc;
            errAcc += errAdj;

            if (errAcc <= errAccTmp)
            {
                yy0 = y0p1;
                y0p1++;
            }

            xx0 += xDir;
            weight = (errAcc >> intShift) & 255;
            Helpers::pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(yy0), r, g, b, a,
                                     255 - weight);
            Helpers::pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(y0p1), r, g, b, a, weight);
        }
    }

    if (draw_endpoint) pixelRGBA(renderer, x2, y2, r, g, b, a);
}

void Helpers::drawQuadrants(Framebuffer renderer, short x, short y, short dx, short dy, int f, unsigned int color)
{
    short xPdx, xMdx;
    short yPdy, yMdy;

    if (dx == 0)
        if (dy == 0) Helpers::pixel(renderer, x, y, color);
        else
        {
            yPdy = static_cast<short>(y + dy);
            yMdy = static_cast<short>(y - dy);

            if (f) Line(renderer, x, yMdy, x, yPdy).draw(color);
            else
            {
                Helpers::pixel(renderer, x, yPdy, color);
                Helpers::pixel(renderer, x, yMdy, color);
            }
        }
    else
    {
        xPdx = static_cast<short>(x + dx);
        xMdx = static_cast<short>(x - dx);
        yPdy = static_cast<short>(y + dy);
        yMdy = static_cast<short>(y - dy);

        if (f)
        {
            Line(renderer, xPdx, yMdy, xPdx, yPdy).draw(color);
            Line(renderer, xMdx, yMdy, xMdx, yPdy).draw(color);
        } else
        {
            Helpers::pixel(renderer, xPdx, yPdy, color);
            Helpers::pixel(renderer, xMdx, yPdy, color);
            Helpers::pixel(renderer, xPdx, yMdy, color);
            Helpers::pixel(renderer, xMdx, yMdy, color);
        }
    }
}

void
Helpers::ellipseRGBA(Framebuffer renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                     unsigned char b, unsigned char a, int f)
{
    int rxi, ryi, rx2, ry2, rx22, ry22, error, curX, curY, curXp1, curYm1, scrX, scrY, oldX, oldY, deltaX, deltaY, ellipseLimit;
    unsigned int color = (r << 24) | (g << 16) | (b << 8) | a;

    if (rx < 0 || ry < 0) return Helpers::warn("Unable to draw ellipse with negative radius");
    if (rx == 0)
        ry == 0 ? Helpers::pixel(renderer, x, y, color) : Line(renderer, x, static_cast<short>(y - ry),
                                                               x, static_cast<short>(y + ry)).draw(color);
    else if (ry == 0) Line(renderer, static_cast<short>(x - rx), y, static_cast<short>(x + rx), y).draw(color);

    rxi = rx;
    ryi = ry;
    ellipseLimit = ELLIPSE_LIMIT / (rxi >= 512 || ryi >= 512 ? 4 : rxi >= 256 || ryi >= 256 ? 2 : 1);

    oldX = scrX = 0;
    oldY = scrY = ryi;
    drawQuadrants(renderer, x, y, 0, ry, f, color);

    rxi *= ellipseLimit;
    ryi *= ellipseLimit;
    rx2 = rxi * rxi;
    rx22 = rx2 + rx2;
    ry2 = ryi * ryi;
    ry22 = ry2 + ry2;
    curX = 0;
    curY = ryi;
    deltaX = 0;
    deltaY = rx22 * curY;

    error = ry2 - rx2 * ryi + rx2 / 4;
    while (deltaX <= deltaY)
    {
        curX++;
        deltaX += ry22;

        error += deltaX + ry2;
        if (error >= 0)
        {
            curY--;
            deltaY -= rx22;
            error -= deltaY;
        }

        scrX = curX / ellipseLimit;
        scrY = curY / ellipseLimit;

        if ((scrX != oldX && scrY == oldY) || (scrX != oldX && scrY != oldY))
        {
            drawQuadrants(renderer, x, y, static_cast<short>(scrX), static_cast<short>(scrY), f, color);

            oldX = scrX;
            oldY = scrY;
        }
    }

    if (curY > 0)
    {
        curXp1 = curX + 1;
        curYm1 = curY - 1;
        error = ry2 * curX * curXp1 + ((ry2 + 3) / 4) + rx2 * curYm1 * curYm1 - rx2 * ry2;

        while (curY > 0)
        {
            curY--;
            deltaY -= rx22;

            error += rx2;
            error -= deltaY;

            if (error <= 0)
            {
                curX++;
                deltaX += ry22;
                error += deltaX;
            }

            scrX = curX / ellipseLimit;
            scrY = curY / ellipseLimit;

            if ((scrX != oldX && scrY == oldY) || (scrX != oldX && scrY != oldY))
            {
                oldY--;
                for (; oldY >= scrY; --oldY)
                {
                    drawQuadrants(renderer, x, y, static_cast<short>(scrX), static_cast<short>(oldY), f, color);
                    if (f) oldY = scrY - 1;
                }

                oldX = scrX;
                oldY = scrY;
            }
        }

        if (!f)
        {
            oldY--;
            for (; oldY >= 0; --oldY)
                drawQuadrants(renderer, x, y, static_cast<short>(scrX), static_cast<short>(oldY), f, color);
        }
    }
}

void Helpers::pieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
                      unsigned char g, unsigned char b, unsigned char a, unsigned char filled)
{
    int numPoints, i;
    double angle, startAngle, endAngle, deltaAngle, dr;
    short* vx, * vy;

    if (rad < 0) return Helpers::warn("Unable to draw pie with negative radius");
    if (rad == 0) pixelRGBA(renderer, x, y, r, g, b, a);

    start = static_cast<short>(start % 360);
    end = static_cast<short>(end % 360);
    dr = static_cast<double>(rad);
    deltaAngle = 3.0 / dr;

    startAngle = static_cast<double>(start * (2.0 * M_PI / 360.0));
    endAngle = static_cast<double>(end * (2.0 * M_PI / 360.0));
    if (start > end) endAngle += 2.0 * M_PI;

    numPoints = 2;
    angle = startAngle;

    while (angle < endAngle)
    {
        angle += deltaAngle;
        numPoints++;
    }

    vx = vy = reinterpret_cast<short*>(malloc(2 * sizeof(short) * numPoints));
    if (vx == nullptr) return Helpers::warn("Unable to allocate memory for pie");
    vy += numPoints;

    vx[0] = x;
    vy[0] = y;

    angle = startAngle;
    vx[1] = static_cast<short>(x + dr * cos(angle));
    vy[1] = static_cast<short>(y + dr * sin(angle));

    if (numPoints < 3) lineRGBA(renderer, vx[0], vy[0], vx[1], vy[1], r, g, b, a);
    else
    {
        i = 2;
        angle = startAngle;

        while (angle < endAngle)
        {
            angle += deltaAngle;
            if (angle > endAngle) angle = endAngle;

            vx[i] = static_cast<short>(x + dr * cos(angle));
            vy[i] = static_cast<short>(y + dr * sin(angle));
            i++;
        }

        filled ? filledPolygonRGBA(renderer, vx, vy, numPoints, r, g, b, a) : polygonRGBA(renderer, vx, vy, numPoints,
                                                                                          r, g, b, a);
    }

    free(vx);
}

void Helpers::polygon(Framebuffer renderer, const short* vx, const short* vy, int n, unsigned int color)
{
    int i, nn;
    std::pair<short, short>* points;

    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon");

    nn = n + 1;
    points = reinterpret_cast<std::pair<short, short>*>(malloc(sizeof(std::pair<short, short>) * nn));
    if (points == nullptr) return Helpers::warn("Unable to allocate memory for polygon");

    for (i = 0; i < n; ++i)
    {
        points[i].first = vx[i];
        points[i].second = vy[i];
    }

    points[n].first = vx[0];
    points[n].second = vy[0];

    Line(renderer, points[0].first, points[0].second, points[1].first, points[1].second).draw(color);
    for (i = 1; i < n; ++i)
        Line(renderer, points[i].first, points[i].second, points[i + 1].first, points[i + 1].second).draw(color);

    free(points);
}

auto Helpers::compareInt(const void* a, const void* b) -> int
{
    return *reinterpret_cast<const int*>(a) - *reinterpret_cast<const int*>(b);
}

auto Helpers::evaluateBezier(double* data, int nData, double t) -> double
{
    double mu, result;
    int n, k, kn, nn, nkn;
    double blend, muK, munK;

    if (t < 0.0) return data[0];
    if (t >= static_cast<double>(nData)) return data[nData - 1];

    mu = t / static_cast<double>(nData);
    n = nData - 1;
    result = 0.0;
    muK = 1;
    munK = pow(1 - mu, static_cast<double>(n));

    for (k = 0; k <= n; ++k)
    {
        nn = n;
        kn = k;
        nkn = n - k;
        blend = muK * munK;
        muK *= mu;
        munK /= (1 - mu);

        while (nn >= 1)
        {
            blend *= nn;
            nn--;

            if (kn > 1)
            {
                blend /= static_cast<double>(kn);
                kn--;
            }

            if (nkn > 1)
            {
                blend /= static_cast<double>(nkn);
                nkn--;
            }
        }

        result += data[k] * blend;
    }

    return result;
}

void pixelColor(const Framebuffer &renderer, short x, short y, unsigned int color)
{
    pixelRGBA(renderer, x, y, color >> 24, color >> 16, color >> 8, color);
}

void pixelRGBA(Framebuffer renderer, short x, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a)
{
    Pixel(renderer, x, y).draw((r << 24) | (g << 16) | (b << 8) | a);
}

void hlineColor(const Framebuffer &renderer, short x1, short x2, short y, unsigned int color)
{
    hlineRGBA(renderer, x1, x2, y, color >> 24, color >> 16, color >> 8, color);
}

void hlineRGBA(Framebuffer renderer, short x1, short x2, short y, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a)
{
    Line(renderer, x1, y, x2, y).draw((r << 24) | (g << 16) | (b << 8) | a);
}

void vlineColor(const Framebuffer &renderer, short x, short y1, short y2, unsigned int color)
{
    vlineRGBA(renderer, x, y1, y2, color >> 24, color >> 16, color >> 8, color);
}

void vlineRGBA(Framebuffer renderer, short x, short y1, short y2, unsigned char r, unsigned char g, unsigned char b,
               unsigned char a)
{
    Line(renderer, x, y1, x, y2).draw((r << 24) | (g << 16) | (b << 8) | a);
}

void rectangleColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color)
{
    rectangleRGBA(renderer, x1, y1, x2, y2, color >> 24, color >> 16, color >> 8, color);
}

void rectangleRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a)
{
    short tmp;

    if (x1 == x2)
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
    else if (y1 == y2) hlineRGBA(renderer, x1, x2, y1, r, g, b, a);

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    Rectangle(renderer, x1, y1, static_cast<float>(x2 - x1), static_cast<float>(y2 - y1))
        .draw((r << 24) | (g << 16) | (b << 8) | a);
}

void roundedRectangleColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad,
                           unsigned int color)
{
    roundedRectangleRGBA(renderer, x1, y1, x2, y2, rad, color >> 24, color >> 16, color >> 8, color);
}

void
roundedRectangleRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad, unsigned char r,
                     unsigned char g, unsigned char b, unsigned char a)
{
    short tmp, w, h, xx1, xx2, yy1, yy2;

    if (rad < 0) return Helpers::warn("Unable to draw rounded rectangle with negative radius");
    if (rad <= 1) rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, a);

    if (x1 == x2)
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
    else if (y1 == y2) hlineRGBA(renderer, x1, x2, y1, r, g, b, a);

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    w = static_cast<short>(x2 - x1);
    h = static_cast<short>(y2 - y1);

    if (rad * 2 > w) rad = static_cast<short>(w / 2);
    if (rad * 2 > h) rad = static_cast<short>(h / 2);

    xx1 = static_cast<short>(x1 + rad);
    xx2 = static_cast<short>(x2 - rad);
    yy1 = static_cast<short>(y1 + rad);
    yy2 = static_cast<short>(y2 - rad);

    arcRGBA(renderer, xx1, yy1, rad, 180, 270, r, g, b, a);
    arcRGBA(renderer, xx2, yy1, rad, 270, 360, r, g, b, a);
    arcRGBA(renderer, xx1, yy2, rad, 90, 180, r, g, b, a);
    arcRGBA(renderer, xx2, yy2, rad, 0, 90, r, g, b, a);

    if (xx1 <= xx2)
    {
        hlineRGBA(renderer, xx1, xx2, y1, r, g, b, a);
        hlineRGBA(renderer, xx1, xx2, y2, r, g, b, a);
    }
    if (yy1 <= yy2)
    {
        vlineRGBA(renderer, x1, yy1, yy2, r, g, b, a);
        vlineRGBA(renderer, x2, yy1, yy2, r, g, b, a);
    }
}

void boxColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color)
{
    boxRGBA(renderer, x1, y1, x2, y2, color >> 24, color >> 16, color >> 8, color);
}

void boxRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
             unsigned char b, unsigned char a)
{
    short tmp;

    if (x1 == x2)
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
    else if (y1 == y2) hlineRGBA(renderer, x1, x2, y1, r, g, b, a);

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    Rectangle(renderer, x1, y1, static_cast<float>(x2 - x1 + 1), static_cast<float>(y2 - y1 + 1))
        .draw((r << 24) | (g << 16) | (b << 8) | a);
}

void roundedBoxColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short rad,
                     unsigned int color)
{
    roundedBoxRGBA(renderer, x1, y1, x2, y2, rad, color >> 24, color >> 16, color >> 8, color);
}

void roundedBoxRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, short rad, unsigned char r,
                    unsigned char g, unsigned char b, unsigned char a)
{
    short w, h, diameter, tmp, cx = 0, cy = rad, ocx = static_cast<short>(0xffff), ocy = static_cast<short>(0xffff),
        df = static_cast<short>(1 - rad), d_e = 3, d_se = static_cast<short>(-2 * rad + 5), xPcx, xMcx, xPcy, xMcy,
        yPcy, yMcy, yPcx, yMcx, x, y, dx, dy;

    if (rad < 0) return Helpers::warn("Unable to draw rounded box with negative radius");
    if (rad <= 1) boxRGBA(renderer, x1, y1, x2, y2, r, g, b, a);

    if (x1 == x2)
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
    else if (y1 == y2) hlineRGBA(renderer, x1, x2, y1, r, g, b, a);

    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    w = static_cast<short>(x2 - x1 + 1);
    h = static_cast<short>(y2 - y1 + 1);
    diameter = static_cast<short>(rad + rad);

    if (diameter > w)
    {
        rad = static_cast<short>(w / 2);
        diameter = static_cast<short>(rad + rad);
    }
    if (diameter > h) rad = static_cast<short>(h / 2);

    x = static_cast<short>(x1 + rad);
    y = static_cast<short>(y1 + rad);
    dx = static_cast<short>(x2 - x1 - rad - rad);
    dy = static_cast<short>(y2 - y1 - rad - rad);

    while (cx <= cy)
    {
        xPcx = static_cast<short>(x + cx);
        xMcx = static_cast<short>(x - cx);
        xPcy = static_cast<short>(x + cy);
        xMcy = static_cast<short>(x - cy);

        if (ocy != cy)
        {
            if (cy > 0)
            {
                yPcy = static_cast<short>(y + cy);
                yMcy = static_cast<short>(y - cy);

                Line(renderer, xMcx, yMcy, static_cast<float>(xPcx + dx), yMcy)
                    .draw((r << 24) | (g << 16) | (b << 8) | a);
                Line(renderer, xMcx, static_cast<float>(yPcy + dy), static_cast<float>(xPcx + dx),
                     static_cast<float>(yPcy + dy)).draw((r << 24) | (g << 16) | (b << 8) | a);
            } else
                Line(renderer, xMcx, y, static_cast<float>(xPcx + dx), y)
                    .draw((r << 24) | (g << 16) | (b << 8) | a);

            ocy = cy;
        }

        if (ocx != cx)
        {
            if (cx != cy)
            {
                if (cx > 0)
                {
                    yPcx = static_cast<short>(y + cx);
                    yMcx = static_cast<short>(y - cx);

                    Line(renderer, xMcy, yMcx, static_cast<float>(xPcy + dx), yMcx)
                        .draw((r << 24) | (g << 16) | (b << 8) | a);
                    Line(renderer, xMcy, static_cast<float>(yPcx + dy), static_cast<float>(xPcy + dx),
                         static_cast<float>(yPcx + dy)).draw((r << 24) | (g << 16) | (b << 8) | a);
                } else
                    Line(renderer, xMcy, y, static_cast<float>(xPcy + dx), y)
                        .draw((r << 24) | (g << 16) | (b << 8) | a);
            }

            ocx = cx;
        }

        if (df < 0)
        {
            df = static_cast<short>(df + d_e);
            d_e += 2;
            d_se += 2;
        } else
        {
            df = static_cast<short>(df + d_se);
            d_e += 2;
            d_se += 4;
            cy--;
        }

        cx++;
    }

    if (dx > 0 && dy > 0)
        boxRGBA(renderer, x1, static_cast<short>(y1 + rad + 1), x2, static_cast<short>(y2 - rad), r, g, b, a);
}

void lineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color)
{
    lineRGBA(renderer, x1, y1, x2, y2, color >> 24, color >> 16, color >> 8, color);
}

void lineRGBA(Framebuffer renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
              unsigned char b, unsigned char a)
{
    Line(renderer, x1, y1, x2, y2).draw((r << 24) | (g << 16) | (b << 8) | a);
}

void aalineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned int color)
{
    Helpers::aalineRGBA(renderer, x1, y1, x2, y2, color >> 24, color >> 16, color >> 8, color, true);
}

void aalineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a)
{
    Helpers::aalineRGBA(renderer, x1, y1, x2, y2, r, g, b, a, true);
}

void circleColor(const Framebuffer &renderer, short x, short y, short rad, unsigned int color)
{
    circleRGBA(renderer, x, y, rad, color >> 24, color >> 16, color >> 8, color);
}

void
circleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g, unsigned char b,
           unsigned char a)
{
    ellipseRGBA(renderer, x, y, rad, rad, r, g, b, a);
}

void arcColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned int color)
{
    arcRGBA(renderer, x, y, rad, start, end, color >> 24, color >> 16, color >> 8, color);
}

void arcRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a)
{
    int startOct, endOct, oct, stopValStart = 0, stopValEnd = 0;
    short cx = 0, cy = rad, df = static_cast<short>(1 - rad), d_e = 3, d_se = static_cast<short>(-2 * rad + 5), xPcx,
        xMcx, xPcy, xMcy, yPcy, yMcy, yPcx, yMcx;
    unsigned char drawOctant;
    double dStart, dEnd, temp = 0.0;

    if (rad < 0) return Helpers::warn("Unable to draw arc with negative radius");
    if (rad == 0) pixelRGBA(renderer, x, y, r, g, b, a);

    drawOctant = 0;
    start %= 360;
    end %= 360;

    while (start < 0) start += 360;
    while (end < 0) end += 360;

    start %= 360;
    end %= 360;
    startOct = start / 45;
    endOct = end / 45;
    oct = startOct % 8;

    while (oct != endOct)
    {
        if (oct == startOct)
        {
            dStart = static_cast<double>(start);
            switch (oct)
            {
                case 0:
                case 3:
                    temp = sin(dStart * M_PI / 180.0);
                    break;
                case 1:
                case 6:
                    temp = cos(dStart * M_PI / 180.0);
                    break;
                case 2:
                case 5:
                    temp = -cos(dStart * M_PI / 180.0);
                    break;
                case 4:
                case 7:
                    temp = -sin(dStart * M_PI / 180.0);
                    break;
                default:
                    break;
            }

            temp *= rad;
            stopValStart = static_cast<int>(temp);
            oct % 2 ? drawOctant |= (1 << oct) : drawOctant &= 255 - (1 << oct);
        }

        if (oct == endOct)
        {
            dEnd = static_cast<double>(end);
            switch (oct)
            {
                case 0:
                case 3:
                    temp = sin(dEnd * M_PI / 180);
                    break;
                case 1:
                case 6:
                    temp = cos(dEnd * M_PI / 180);
                    break;
                case 2:
                case 5:
                    temp = -cos(dEnd * M_PI / 180);
                    break;
                case 4:
                case 7:
                    temp = -sin(dEnd * M_PI / 180);
                    break;
                default:
                    break;
            }

            temp *= rad;
            stopValEnd = static_cast<int>(temp);

            if (startOct == endOct) start > end ? drawOctant = 255 : drawOctant &= 255 - (1 << oct);
            else if (oct % 2) drawOctant &= 255 - (1 << oct);
            else drawOctant |= (1 << oct);
        } else if (oct != startOct) drawOctant |= (1 << oct);

        oct = (oct + 1) % 8;
    }

    while (cx <= cy)
    {
        yPcy = static_cast<short>(y + cy);
        yMcy = static_cast<short>(y - cy);

        if (cx > 0)
        {
            xPcx = static_cast<short>(x + cx);
            xMcx = static_cast<short>(x - cx);

            if (drawOctant & 4) Helpers::pixel(renderer, xMcx, yPcy, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 2) Helpers::pixel(renderer, xPcx, yPcy, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 32) Helpers::pixel(renderer, xMcx, yMcy, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 64) Helpers::pixel(renderer, xPcx, yMcy, (r << 24) | (g << 16) | (b << 8) | a);
        } else
        {
            if (drawOctant & 96) Helpers::pixel(renderer, x, yMcy, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 6) Helpers::pixel(renderer, x, yPcy, (r << 24) | (g << 16) | (b << 8) | a);
        }

        xPcy = static_cast<short>(x + cy);
        xMcy = static_cast<short>(x - cy);

        if (cx > 0 && cx != cy)
        {
            yPcx = static_cast<short>(y + cx);
            yMcx = static_cast<short>(y - cx);

            if (drawOctant & 8) Helpers::pixel(renderer, xMcy, yPcx, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 1) Helpers::pixel(renderer, xPcy, yPcx, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 16) Helpers::pixel(renderer, xMcy, yMcx, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 128) Helpers::pixel(renderer, xPcy, yMcx, (r << 24) | (g << 16) | (b << 8) | a);
        } else if (cx == 0)
        {
            if (drawOctant & 24) Helpers::pixel(renderer, xMcy, y, (r << 24) | (g << 16) | (b << 8) | a);
            if (drawOctant & 129) Helpers::pixel(renderer, xPcy, y, (r << 24) | (g << 16) | (b << 8) | a);
        }

        if (stopValStart == cx)
            drawOctant & (1 << startOct) ? drawOctant &= 255 - (1 << startOct) : drawOctant |= (1 << startOct);
        if (stopValEnd == cx)
            drawOctant & (1 << endOct) ? drawOctant &= 255 - (1 << endOct) : drawOctant |= (1 << endOct);

        if (df < 0)
        {
            df = static_cast<short>(df + d_e);
            d_e += 2;
            d_se += 2;
        } else
        {
            df = static_cast<short>(df + d_se);
            d_e += 2;
            d_se += 4;
            cy--;
        }

        cx++;
    }
}

void aacircleColor(const Framebuffer &renderer, short x, short y, short rad, unsigned int color)
{
    aaellipseRGBA(renderer, x, y, rad, rad, color >> 24, color >> 16, color >> 8, color);
}

void aacircleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a)
{
    aaellipseRGBA(renderer, x, y, rad, rad, r, g, b, a);
}

void ellipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, color >> 24, color >> 16, color >> 8, color, false);
}

void ellipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a, false);
}

void filledCircleColor(const Framebuffer &renderer, short x, short y, short r, unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, r, r, color >> 24, color >> 16, color >> 8, color, true);
}

void filledCircleRGBA(const Framebuffer &renderer, short x, short y, short rad, unsigned char r, unsigned char g,
                      unsigned char b, unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rad, rad, r, g, b, a, true);
}

#if defined(_MSC_VER)
#   ifdef _M_X64
#   include <emmintrin.h>

static __inline long
    lrint(double d)
{
    float f = (float)d;
    return _mm_cvtss_si32(_mm_load_ss(&f));
}

#   elif defined(_M_IX86)

__inline long int lrint (double flt)
{
    int intgr;
    _asm
    {
        fld flt
        fistp intgr
    };

    return intgr;
}
#   elif defined(_M_ARM)
#       include <armintr.h>

__declspec(naked) long int lrint (double flt)
{
    __emit(0xEC410B10);
    __emit(0xEEBD0B40);
    __emit(0xEE100A10);
    __emit(0xE12FFF1E);
}

#   else
#       error lrint() is not implemented or not supported on this architecture.
#   endif
#endif

void aaellipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color)
{
    aaellipseRGBA(renderer, x, y, rx, ry, color >> 24, color >> 16, color >> 8, color);
}

void aaellipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a)
{
    int i, a2, b2, ds, dt, dxt, t, s, d;
    short xp, yp, xs, ys, dyt, overdraw, xx, yy, xc2, yc2;
    double cp, root;
    unsigned char weight, iWeight;

    if (rx < 0 || ry < 0) return Helpers::warn("Unable to draw ellipse with negative radius");
    if (rx == 0)
        ry == 0 ? pixelRGBA(renderer, x, y, r, g, b, a)
                : vlineRGBA(renderer, x, static_cast<short>(y - ry), static_cast<short>(y + ry), r, g, b, a);
    else if (ry == 0) hlineRGBA(renderer, static_cast<short>(x - rx), static_cast<short>(x + rx), y, r, g, b, a);

    a2 = rx * rx;
    b2 = ry * ry;
    ds = 2 * a2;
    dt = 2 * b2;
    xc2 = static_cast<short>(2 * x);
    yc2 = static_cast<short>(2 * y);

    root = sqrt(static_cast<double>(a2 + b2));
    overdraw = static_cast<short>(lrint(root * 0.01) + 1);
    dxt = static_cast<short>(lrint(static_cast<double>(a2 / root)) + overdraw);

    t = 0;
    s = -2 * a2 * ry;
    d = 0;
    xp = x;
    yp = static_cast<short>(y - ry);

    pixelRGBA(renderer, xp, yp, r, g, b, a);
    pixelRGBA(renderer, static_cast<short>(xc2 - xp), yp, r, g, b, a);
    pixelRGBA(renderer, xp, static_cast<short>(yc2 - yp), r, g, b, a);
    pixelRGBA(renderer, static_cast<short>(xc2 - xp), static_cast<short>(yc2 - yp), r, g, b, a);

    for (i = 1; i <= dxt; ++i)
    {
        xp--;
        d += t - b2;

        if (d >= 0) ys = static_cast<short>(yp - 1);
        else if (d - s - a2 > 0)
            if (2 * d - s - a2 >= 0) ys = static_cast<short>(yp + 1);
            else
            {
                ys = yp;
                yp++;
                d -= s + a2;
                s += ds;
            }
        else
        {
            yp++;
            ys = static_cast<short>(yp + 1);
            d -= s + a2;
            s += ds;
        }

        t -= dt;

        if (s != 0)
        {
            cp = (float) abs(d) / (float) abs(s);
            if (cp > 1.0) cp = 1.0;
        } else cp = 1.0;

        weight = static_cast<unsigned char>(cp * 255);
        iWeight = 255 - weight;

        xx = static_cast<short>(xc2 - xp);
        Helpers::pixelRGBAWeight(renderer, xp, yp, r, g, b, a, iWeight);
        Helpers::pixelRGBAWeight(renderer, xx, yp, r, g, b, a, iWeight);

        Helpers::pixelRGBAWeight(renderer, xp, ys, r, g, b, a, weight);
        Helpers::pixelRGBAWeight(renderer, xx, ys, r, g, b, a, weight);

        yy = static_cast<short>(yc2 - yp);
        Helpers::pixelRGBAWeight(renderer, xp, yy, r, g, b, a, iWeight);
        Helpers::pixelRGBAWeight(renderer, xx, yy, r, g, b, a, iWeight);

        yy = static_cast<short>(yc2 - ys);
        Helpers::pixelRGBAWeight(renderer, xp, yy, r, g, b, a, weight);
        Helpers::pixelRGBAWeight(renderer, xx, yy, r, g, b, a, weight);
    }

    dyt = static_cast<short>(lrint(static_cast<double>(b2 / root)) + overdraw);
    for (i = 1; i <= dyt; ++i)
    {
        yp++;
        d -= s + a2;

        if (d <= 0) xs = static_cast<short>(xp + 1);
        else if (d + t - b2 < 0)
        {
            if (2 * d + t - b2 <= 0) xs = static_cast<short>(xp - 1);
            else
            {
                xs = xp;
                xp--;
                d += t - b2;
                t -= dt;
            }
        } else
        {
            xp--;
            xs = static_cast<short>(xp - 1);
            d += t - b2;
            t -= dt;
        }

        s += ds;
        if (t != 0)
        {
            cp = static_cast<double>(abs(d)) / abs(t);
            if (cp > 1.0) cp = 1.0;
        } else cp = 1.0;

        weight = static_cast<unsigned char>(cp * 255);
        iWeight = 255 - weight;

        xx = static_cast<short>(xc2 - xp);
        yy = static_cast<short>(yc2 - yp);
        Helpers::pixelRGBAWeight(renderer, xp, yp, r, g, b, a, iWeight);
        Helpers::pixelRGBAWeight(renderer, xx, yp, r, g, b, a, iWeight);

        Helpers::pixelRGBAWeight(renderer, xp, yy, r, g, b, a, iWeight);
        Helpers::pixelRGBAWeight(renderer, xx, yy, r, g, b, a, iWeight);

        xx = static_cast<short>(xc2 - xs);
        Helpers::pixelRGBAWeight(renderer, xs, yp, r, g, b, a, weight);
        Helpers::pixelRGBAWeight(renderer, xx, yp, r, g, b, a, weight);

        Helpers::pixelRGBAWeight(renderer, xs, yy, r, g, b, a, weight);
        Helpers::pixelRGBAWeight(renderer, xx, yy, r, g, b, a, weight);
    }
}

void filledEllipseColor(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, color >> 24, color >> 16, color >> 8, color, true);
}

void
filledEllipseRGBA(const Framebuffer &renderer, short x, short y, short rx, short ry, unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a, true);
}

void pieColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned int color)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, color >> 24, color >> 16, color >> 8, color, false);
}

void pieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, r, g, b, a, false);
}

void filledPieColor(const Framebuffer &renderer, short x, short y, short rad, short start, short end,
                    unsigned int color)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, color >> 24, color >> 16, color >> 8, color, true);
}

void filledPieRGBA(const Framebuffer &renderer, short x, short y, short rad, short start, short end, unsigned char r,
                   unsigned char g, unsigned char b, unsigned char a)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, r, g, b, a, true);
}

void trigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                 unsigned int color)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    polygonColor(renderer, vx, vy, 3, color);
}

void
trigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3, unsigned char r,
           unsigned char g, unsigned char b, unsigned char a)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    polygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void aatrigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                   unsigned int color)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    aapolygonColor(renderer, vx, vy, 3, color);
}

void
aatrigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3, unsigned char r,
             unsigned char g, unsigned char b, unsigned char a)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    aapolygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void filledTrigonColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                       unsigned int color)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    filledPolygonColor(renderer, vx, vy, 3, color);
}

void filledTrigonRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, short x3, short y3,
                      unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    filledPolygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void polygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color)
{
    polygonRGBA(renderer, vx, vy, n, color >> 24, color >> 16, color >> 8, color);
}

void polygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a)
{
    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");

    Helpers::polygon(renderer, vx, vy, n, (r << 24) | (g << 16) | (b << 8) | a);
}

void aapolygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color)
{
    aapolygonRGBA(renderer, vx, vy, n, color >> 24, color >> 16, color >> 8, color);
}

void
aapolygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r, unsigned char g,
              unsigned char b, unsigned char a)
{
    int i;
    const short* x1, * y1, * x2, * y2;

    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");

    x1 = x2 = vx;
    y1 = y2 = vy;
    x2++;
    y2++;

    for (i = 1; i < n; ++i)
    {
        Helpers::aalineRGBA(renderer, *x1, *y1, *x2, *y2, r, g, b, a, false);

        x1 = x2;
        y1 = y2;
        x2++;
        y2++;
    }
}

void filledPolygonRGBAMT(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r,
                         unsigned char g, unsigned char b, unsigned char a, int** polyInts, int* polyAllocated)
{
    int i, y, xa, xb, miny, maxy, x1, y1, x2, y2, ind1, ind2, ints, * polygonInts, * polygonIntsNew,
        polygonAllocated = 0;
    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");

    if (polyInts != nullptr && polyAllocated != nullptr)
    {
        polygonInts = *polyInts;
        polygonAllocated = *polyAllocated;
    }

    if (!polygonAllocated)
    {
        polygonInts = reinterpret_cast<int*>(malloc(sizeof(int) * n));
        polygonAllocated = n;
    } else if (polygonAllocated < n)
    {
        polygonIntsNew = reinterpret_cast<int*>(realloc(polygonInts, sizeof(int) * n));
        if (!polygonIntsNew)
        {
            if (!polygonInts)
            {
                free(polygonInts);
                polygonInts = nullptr;
            }

            polygonAllocated = 0;
        } else
        {
            polygonInts = polygonIntsNew;
            polygonAllocated = n;
        }
    }

    if (polygonInts == nullptr) polygonAllocated = 0;
    if (polyInts != nullptr) *polyInts = polygonInts;
    if (polyAllocated != nullptr) *polyAllocated = polygonAllocated;
    if (polygonInts == nullptr) return Helpers::warn("Unable to allocate space for polygon fill");

    miny = vy[0];
    maxy = vy[0];

    for (i = 1; (i < n); ++i)
        if (vy[i] < miny) miny = vy[i];
        else if (vy[i] > maxy) maxy = vy[i];

    for (y = miny; (y <= maxy); ++y)
    {
        ints = 0;
        for (i = 0; (i < n); ++i)
        {
            ind1 = i ? i - 1 : n - 1;
            ind2 = i ? i : 0;
            y1 = vy[ind1];
            y2 = vy[ind2];

            if (y1 < y2)
            {
                x1 = vx[ind1];
                x2 = vx[ind2];
            } else if (y1 > y2)
            {
                y2 = vy[ind1];
                y1 = vy[ind2];
                x2 = vx[ind1];
                x1 = vx[ind2];
            } else continue;

            if ((y >= y1 && y < y2) || (y == maxy && y > y1 && y <= y2))
                polygonInts[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
        }

        qsort(polygonInts, ints, sizeof(int), Helpers::compareInt);

        for (i = 0; (i < ints); i += 2)
        {
            xa = polygonInts[i] + 1;
            xa = (xa >> 16) + ((xa & 32768) >> 15);
            xb = polygonInts[i + 1] - 1;
            xb = (xb >> 16) + ((xb & 32768) >> 15);

            hlineRGBA(renderer, static_cast<short>(xa), static_cast<short>(xb), static_cast<short>(y), r, g, b, a);
        }
    }
}

void filledPolygonColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned int color)
{
    filledPolygonRGBAMT(renderer, vx, vy, n, color >> 24, color >> 16, color >> 8, color, nullptr, nullptr);
}

void filledPolygonRGBA(const Framebuffer &renderer, const short* vx, const short* vy, int n, unsigned char r,
                       unsigned char g, unsigned char b, unsigned char a)
{
    filledPolygonRGBAMT(renderer, vx, vy, n, r, g, b, a, nullptr, nullptr);
}

void bezierColor(const Framebuffer &renderer, const short* vx, const short* vy, int n, int s, unsigned int color)
{
    bezierRGBA(renderer, vx, vy, n, s, color >> 24, color >> 16, color >> 8, color);
}

void bezierRGBA(Framebuffer renderer, const short* vx, const short* vy, int n, int s, unsigned char r,
                unsigned char g, unsigned char b, unsigned char a)
{
    int i;
    double* x, * y, t, stepSize;
    short x1, y1, x2, y2;

    if (n < 3 || s < 2) return Helpers::warn("Unable to draw bezier curve with less than 3 points or 2 subdivisions");
    stepSize = static_cast<double>(1.0 / s);

    x = reinterpret_cast<double*>(malloc(sizeof(double) * (n + 1)));
    y = reinterpret_cast<double*>(malloc(sizeof(double) * (n + 1)));

    if (x == nullptr || y == nullptr)
    {
        if (x != nullptr) free(x);
        if (y != nullptr) free(y);

        return Helpers::warn("Unable to allocate space for bezier curve");
    }

    for (i = 0; i < n; ++i)
    {
        x[i] = static_cast<double>(vx[i]);
        y[i] = static_cast<double>(vy[i]);
    }

    x[n] = static_cast<double>(vx[0]);
    y[n] = static_cast<double>(vy[0]);

    t = 0.0;
    x1 = static_cast<short>(lrint(Helpers::evaluateBezier(x, n + 1, t)));
    y1 = static_cast<short>(lrint(Helpers::evaluateBezier(y, n + 1, t)));

    for (i = 0; i <= n * s; ++i)
    {
        t += stepSize;
        x2 = static_cast<short>(Helpers::evaluateBezier(x, n, t));
        y2 = static_cast<short>(Helpers::evaluateBezier(y, n, t));
        Line(renderer, x1, y1, x2, y2).draw((r << 24) | (g << 16) | (b << 8) | a);

        x1 = x2;
        y1 = y2;
    }

    free(x);
    free(y);
}

void thickLineColor(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char width,
                    unsigned int color)
{
    thickLineRGBA(renderer, x1, y1, x2, y2, width, color >> 24, color >> 16, color >> 8, color);
}

void
thickLineRGBA(const Framebuffer &renderer, short x1, short y1, short x2, short y2, unsigned char width, unsigned char r,
              unsigned char g, unsigned char b, unsigned char a)
{
    int wh;
    double dx, dy, dx1, dy1, dx2, dy2, l, wl2, nx, ny, ang, adj;

    if (width < 1) return Helpers::warn("Unable to draw thick line with width less than 1");

    if (x1 == x2 && y1 == y2)
    {
        wh = width / 2;
        boxRGBA(renderer, static_cast<short>(x1 - wh), static_cast<short>(y1 - wh),
                static_cast<short>(x2 + width), static_cast<short>(y2 + width), r, g, b, a);
    }

    if (width == 1) lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);

    dx = static_cast<double>(x2 - x1);
    dy = static_cast<double>(y2 - y1);

    l = sqrt(dx * dx + dy * dy);
    ang = atan2(dx, dy);
    adj = 0.1 + 0.9 * fabs(cos(2.0 * ang));
    wl2 = static_cast<double>(width - adj) / (2.0 * l);
    nx = dx * wl2;
    ny = dy * wl2;

    dx1 = static_cast<double>(x1);
    dy1 = static_cast<double>(y1);
    dx2 = static_cast<double>(x2);
    dy2 = static_cast<double>(y2);

    short px[4] = {static_cast<short>(dx1 + ny), static_cast<short>(dx1 - ny), static_cast<short>(dx2 - ny),
                   static_cast<short>(dx2 + ny)};
    short py[4] = {static_cast<short>(dy1 - nx), static_cast<short>(dy1 + nx), static_cast<short>(dy2 + nx),
                   static_cast<short>(dy2 - nx)};

    filledPolygonRGBA(renderer, px, py, 4, r, g, b, a);
}
