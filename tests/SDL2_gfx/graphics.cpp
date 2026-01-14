#include "graphics.h"

void Helpers::warn(const std::string& message) { std::cerr << "Warning: " << message << std::endl; }

void Helpers::pixel(Framebuffer& renderer, const short x, const short y, const unsigned int color)
{
    Pixel(renderer, x, y).draw(color);
}

void Helpers::pixelRGBAWeight(Framebuffer& renderer, const short x, const short y, const unsigned char r,
                              const unsigned char g, const unsigned char b, unsigned char a, const unsigned int weight)
{
    unsigned int ax = a;
    ax = ax * weight >> 8;
    a = ax > 255 ? 255 : static_cast<unsigned char>(ax & 0x000000ff);

    pixelRGBA(renderer, x, y, r, g, b, a);
}

void Helpers::aalineRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                         const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a,
                         const bool drawEndpoint)
{
    int xDir;
    unsigned int errAdj, errAccTmp, weight;

    int xx0 = x1, yy0 = y1;
    int xx1 = x2, yy1 = y2;

    if (yy0 > yy1)
    {
        std::swap(yy0, yy1);
        std::swap(xx0, xx1);
    }

    int dx = xx1 - xx0;
    int dy = yy1 - yy0;

    if (dx >= 0) xDir = 1;
    else
    {
        xDir = -1;
        dx = -dx;
    }

    if (dx == 0)
    {
        if (drawEndpoint) vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
        else if (dy > 0) vlineRGBA(renderer, x1, static_cast<short>(yy0), static_cast<short>(yy0 + dy), r, g, b, a);
        else pixelRGBA(renderer, x1, y1, r, g, b, a);

        return;
    }

    if (dy == 0)
    {
        if (drawEndpoint) hlineRGBA(renderer, x1, x2, y1, r, g, b, a);
        else if (dx > 0)
            hlineRGBA(renderer, static_cast<short>(xx0), static_cast<short>(xx0 + xDir * dx), y1, r, g, b,
                      a);
        else pixelRGBA(renderer, x1, y1, r, g, b, a);

        return;
    }

    if (dx == dy && drawEndpoint)
    {
        lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
        return;
    }

    unsigned int errAcc = 0;
    const unsigned int intShift = 32 - AA_BITS;

    pixelRGBA(renderer, x1, y1, r, g, b, a);

    if (dy > dx)
    {
        errAdj = ((dx << 16) / dy) << 16;
        int x0pxDir = xx0 + xDir;

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
            pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(yy0), r, g, b, a, 255 - weight);
            pixelRGBAWeight(renderer, static_cast<short>(x0pxDir), static_cast<short>(yy0), r, g, b, a, weight);
        }
    }
    else
    {
        errAdj = ((dy << 16) / dx) << 16;
        int y0p1 = yy0 + 1;

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
            pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(yy0), r, g, b, a, 255 - weight);
            pixelRGBAWeight(renderer, static_cast<short>(xx0), static_cast<short>(y0p1), r, g, b, a, weight);
        }
    }

    if (drawEndpoint) pixelRGBA(renderer, x2, y2, r, g, b, a);
}

void Helpers::drawQuadrants(Framebuffer& renderer, const short x, const short y, const short dx, const short dy,
                            const int f, const unsigned int color)
{
    short yPdy, yMdy;

    if (dx == 0)
        if (dy == 0) pixel(renderer, x, y, color);
        else
        {
            yPdy = static_cast<short>(y + dy);
            yMdy = static_cast<short>(y - dy);

            if (f) Line(renderer, x, yMdy, x, yPdy).draw(color);
            else
            {
                pixel(renderer, x, yPdy, color);
                pixel(renderer, x, yMdy, color);
            }
        }
    else
    {
        const auto xPdx = static_cast<short>(x + dx);
        const auto xMdx = static_cast<short>(x - dx);
        yPdy = static_cast<short>(y + dy);
        yMdy = static_cast<short>(y - dy);

        if (f)
        {
            Line(renderer, xPdx, yMdy, xPdx, yPdy).draw(color);
            Line(renderer, xMdx, yMdy, xMdx, yPdy).draw(color);
        }
        else
        {
            pixel(renderer, xPdx, yPdy, color);
            pixel(renderer, xMdx, yPdy, color);
            pixel(renderer, xPdx, yMdy, color);
            pixel(renderer, xMdx, yMdy, color);
        }
    }
}

void Helpers::ellipseRGBA(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                          const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a,
                          const int f)
{
    int rxi, ryi, rx2, ry2, rx22, ry22, error, curX, curY, scrX, scrY, oldX, oldY, deltaX, deltaY,
        ellipseLimit;
    const unsigned int color = (a << 24) | (r << 16) | (g << 8) | b;

    if (rx < 0 || ry < 0) return warn("Unable to draw ellipse with negative radius");
    if (rx == 0)
        ry == 0
            ? pixel(renderer, x, y, color)
            : Line(renderer, x, static_cast<short>(y - ry), x, static_cast<short>(y + ry)).draw(color);
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
        int curYm1;
        int curXp1;
        curXp1 = curX + 1;
        curYm1 = curY - 1;
        error = ry2 * curX * curXp1 + (ry2 + 3) / 4 + rx2 * curYm1 * curYm1 - rx2 * ry2;

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
                drawQuadrants(renderer, x, y, static_cast<short>(scrX), static_cast<short>(oldY),
                              f, color);
        }
    }
}

void Helpers::pieRGBA(Framebuffer& renderer, const short x, const short y, const short rad, short start,
                      short end, const unsigned char r, const unsigned char g, const unsigned char b,
                      const unsigned char a, const unsigned char filled)
{
    short* vy;

    if (rad < 0) return warn("Unable to draw pie with negative radius");
    if (rad == 0) pixelRGBA(renderer, x, y, r, g, b, a);

    start = static_cast<short>(start % 360);
    end = static_cast<short>(end % 360);
    const auto dr = static_cast<double>(rad);
    const double deltaAngle = 3.0 / dr;

    const double startAngle = start * (2.0 * M_PI / 360.0);
    double endAngle = end * (2.0 * M_PI / 360.0);
    if (start > end) endAngle += 2.0 * M_PI;

    int numPoints = 2;
    double angle = startAngle;

    while (angle < endAngle)
    {
        angle += deltaAngle;
        numPoints++;
    }

    short* vx = vy = static_cast<short*>(malloc(2 * sizeof(short) * numPoints));
    if (vx == nullptr) return warn("Unable to allocate memory for pie");
    vy += numPoints;

    vx[0] = x;
    vy[0] = y;

    angle = startAngle;
    vx[1] = static_cast<short>(x + dr * cos(angle));
    vy[1] = static_cast<short>(y + dr * sin(angle));

    if (numPoints < 3) lineRGBA(renderer, vx[0], vy[0], vx[1], vy[1], r, g, b, a);
    else
    {
        int i = 2;
        angle = startAngle;

        while (angle < endAngle)
        {
            angle += deltaAngle;
            if (angle > endAngle) angle = endAngle;

            vx[i] = static_cast<short>(x + dr * cos(angle));
            vy[i] = static_cast<short>(y + dr * sin(angle));
            i++;
        }

        filled
            ? filledPolygonRGBA(renderer, vx, vy, numPoints, r, g, b, a)
            : polygonRGBA(renderer, vx, vy, numPoints, r, g, b, a);
    }

    free(vx);
}

void Helpers::polygon(Framebuffer& renderer, const short* vx, const short* vy, const int n, const unsigned int color)
{
    int i;

    if (vx == nullptr || vy == nullptr || n < 3) return warn("Unable to draw polygon");

    const int nn = n + 1;
    const auto points = static_cast<std::pair<short, short>*>(malloc(
        sizeof(std::pair<short, short>) * nn));
    if (points == nullptr) return warn("Unable to allocate memory for polygon");

    for (i = 0; i < n; ++i)
    {
        points[i].first = vx[i];
        points[i].second = vy[i];
    }

    points[n].first = vx[0];
    points[n].second = vy[0];

    Line(renderer, points[0].first, points[0].second, points[1].first, points[1].second).draw(color);
    for (i = 1; i < n; ++i)
        Line(renderer, points[i].first, points[i].second, points[i + 1].first, points[i + 1].second)
            .draw(color);

    free(points);
}

auto Helpers::compareInt(const void* a, const void* b) -> int
{
    return *static_cast<const int*>(a) - *static_cast<const int*>(b);
}

auto Helpers::evaluateBezier(const double* data, const int nData, const double t) -> double
{
    if (t < 0.0) return data[0];
    if (t >= static_cast<double>(nData)) return data[nData - 1];

    const double mu = t / static_cast<double>(nData);
    const int n = nData - 1;
    double result = 0.0;
    double muK = 1;
    double munK = pow(1 - mu, static_cast<double>(n));

    for (int k = 0; k <= n; ++k)
    {
        int nn = n;
        int kn = k;
        int nkn = n - k;
        double blend = muK * munK;
        muK *= mu;
        munK /= 1 - mu;

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

void pixelColor(Framebuffer& renderer, const short x, const short y, const unsigned int color)
{
    pixelRGBA(renderer, x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void pixelRGBA(Framebuffer& renderer, const short x, const short y, const unsigned char r, const unsigned char g,
               const unsigned char b, const unsigned char a)
{
    Pixel(renderer, x, y).draw((a << 24) | (r << 16) | (g << 8) | b);
}

void hlineColor(Framebuffer& renderer, const short x1, const short x2, const short y, const unsigned int color)
{
    hlineRGBA(renderer, x1, x2, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void hlineRGBA(Framebuffer& renderer, const short x1, const short x2, const short y, const unsigned char r,
               const unsigned char g, const unsigned char b, const unsigned char a)
{
    Line(renderer, x1, y, x2, y).draw((a << 24) | (r << 16) | (g << 8) | b);
}

void vlineColor(Framebuffer& renderer, const short x, const short y1, const short y2, const unsigned int color)
{
    vlineRGBA(renderer, x, y1, y2, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void vlineRGBA(Framebuffer& renderer, const short x, const short y1, const short y2, const unsigned char r,
               const unsigned char g, const unsigned char b, const unsigned char a)
{
    Line(renderer, x, y1, x, y2).draw((a << 24) | (r << 16) | (g << 8) | b);
}

void rectangleColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                    const unsigned int color)
{
    rectangleRGBA(renderer, x1, y1, x2, y2, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                  (color >> 24) & 0xFF);
}

void rectangleRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, const unsigned char r,
                   const unsigned char g, const unsigned char b, const unsigned char a)
{
    short tmp;

    if (x1 == x2)
    {
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
        return;
    }

    if (y1 == y2)
    {
        hlineRGBA(renderer, x1, x2, y1, r, g, b, a);
        return;
    }

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

    Rect(renderer, x1, y1, static_cast<float>(x2 - x1 + 1), static_cast<float>(y2 - y1 + 1), false).draw(
        (a << 24) | (r << 16) | (g << 8) | b);
}

void roundedRectangleColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                           const short rad, const unsigned int color)
{
    roundedRectangleRGBA(renderer, x1, y1, x2, y2, rad, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                         (color >> 24) & 0xFF);
}

void roundedRectangleRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad,
                          const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    short tmp;

    if (rad < 0) return Helpers::warn("Unable to draw rounded rectangle with negative radius");
    if (rad <= 1)
    {
        rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
        return;
    }

    if (x1 == x2)
    {
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
        return;
    }

    if (y1 == y2)
    {
        hlineRGBA(renderer, x1, x2, y1, r, g, b, a);
        return;
    }

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

    const auto w = static_cast<short>(x2 - x1);
    const auto h = static_cast<short>(y2 - y1);

    if (rad * 2 > w) rad = static_cast<short>(w / 2);
    if (rad * 2 > h) rad = static_cast<short>(h / 2);

    const auto xx1 = static_cast<short>(x1 + rad);
    const auto xx2 = static_cast<short>(x2 - rad);
    const auto yy1 = static_cast<short>(y1 + rad);
    const auto yy2 = static_cast<short>(y2 - rad);

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

void boxColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
              const unsigned int color)
{
    boxRGBA(renderer, x1, y1, x2, y2, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void boxRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, const unsigned char r,
             const unsigned char g,
             const unsigned char b, const unsigned char a)
{
    short tmp;

    if (x1 == x2) y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
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

    Rect(renderer, x1, y1, static_cast<float>(x2 - x1 + 1), static_cast<float>(y2 - y1 + 1), true).draw(
        (a << 24) | (r << 16) | (g << 8) | b);
}

void roundedBoxColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                     const short rad, const unsigned int color)
{
    roundedBoxRGBA(renderer, x1, y1, x2, y2, rad, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                   (color >> 24) & 0xFF);
}

void roundedBoxRGBA(Framebuffer& renderer, short x1, short y1, short x2, short y2, short rad, const unsigned char r,
                    const unsigned char g, const unsigned char b, const unsigned char a)
{
    short tmp, cx = 0, cy = rad, ocx = static_cast<short>(0xffff), ocy = static_cast<short>(0xffff), df
              = static_cast<short>(1 - rad), d_e = 3, d_se = static_cast<short>(-2 * rad + 5);

    if (rad < 0) return Helpers::warn("Unable to draw rounded box with negative radius");
    if (rad <= 1)
    {
        boxRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
        return;
    }

    if (x1 == x2)
    {
        y1 == y2 ? pixelRGBA(renderer, x1, y1, r, g, b, a) : vlineRGBA(renderer, x1, y1, y2, r, g, b, a);
        return;
    }

    if (y1 == y2)
    {
        hlineRGBA(renderer, x1, x2, y1, r, g, b, a);
        return;
    }

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

    const auto w = static_cast<short>(x2 - x1 + 1);
    const auto h = static_cast<short>(y2 - y1 + 1);
    auto diameter = static_cast<short>(rad + rad);

    if (diameter > w)
    {
        rad = static_cast<short>(w / 2);
        diameter = static_cast<short>(rad + rad);
    }
    if (diameter > h) rad = static_cast<short>(h / 2);

    const auto x = static_cast<short>(x1 + rad);
    const auto y = static_cast<short>(y1 + rad);
    const auto dx = static_cast<short>(x2 - x1 - rad - rad);
    const auto dy = static_cast<short>(y2 - y1 - rad - rad);

    while (cx <= cy)
    {
        const auto xPcx = static_cast<short>(x + cx);
        const auto xMcx = static_cast<short>(x - cx);
        const auto xPcy = static_cast<short>(x + cy);
        const auto xMcy = static_cast<short>(x - cy);

        if (ocy != cy)
        {
            if (cy > 0)
            {
                const auto yPcy = static_cast<short>(y + cy);
                const auto yMcy = static_cast<short>(y - cy);

                Line(renderer, xMcx, yMcy, static_cast<float>(xPcx + dx), yMcy).draw(
                    (a << 24) | (r << 16) | (g << 8) | b);
                Line(renderer, xMcx, static_cast<float>(yPcy + dy), static_cast<float>(xPcx + dx),
                     static_cast<float>(yPcy + dy)).draw((a << 24) | (r << 16) | (g << 8) | b);
            }
            else
                Line(renderer, xMcx, y, static_cast<float>(xPcx + dx), y).draw(
                    (a << 24) | (r << 16) | (g << 8) | b);

            ocy = cy;
        }

        if (ocx != cx)
        {
            if (cx != cy)
            {
                if (cx > 0)
                {
                    const auto yPcx = static_cast<short>(y + cx);
                    const auto yMcx = static_cast<short>(y - cx);

                    Line(renderer, xMcy, yMcx, static_cast<float>(xPcy + dx), yMcx).draw(
                        (a << 24) | (r << 16) | (g << 8) | b);
                    Line(renderer, xMcy, static_cast<float>(yPcx + dy), static_cast<float>(xPcy + dx),
                         static_cast<float>(yPcx + dy)).draw((a << 24) | (r << 16) | (g << 8) | b);
                }
                else
                    Line(renderer, xMcy, y, static_cast<float>(xPcy + dx), y).
                        draw((a << 24) | (r << 16) | (g << 8) | b);
            }

            ocx = cx;
        }

        if (df < 0)
        {
            df = static_cast<short>(df + d_e);
            d_e += 2;
            d_se += 2;
        }
        else
        {
            df = static_cast<short>(df + d_se);
            d_e += 2;
            d_se += 4;
            cy--;
        }

        cx++;
    }

    if (dx > 0 && dy > 0)
        boxRGBA(renderer, x1, static_cast<short>(y1 + rad + 1), x2, static_cast<short>(y2 - rad), r,
                g, b, a);
}

void lineColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
               const unsigned int color)
{
    lineRGBA(renderer, x1, y1, x2, y2, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void lineRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
              const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    Line(renderer, x1, y1, x2, y2).draw((a << 24) | (r << 16) | (g << 8) | b);
}

void aalineColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                 const unsigned int color)
{
    Helpers::aalineRGBA(renderer, x1, y1, x2, y2, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                        (color >> 24) & 0xFF, true);
}

void aalineRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    Helpers::aalineRGBA(renderer, x1, y1, x2, y2, r, g, b, a, true);
}

void circleColor(Framebuffer& renderer, const short x, const short y, const short rad, const unsigned int color)
{
    circleRGBA(renderer, x, y, rad, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void circleRGBA(Framebuffer& renderer, const short x, const short y, const short rad, const unsigned char r,
                const unsigned char g, const unsigned char b, const unsigned char a)
{
    ellipseRGBA(renderer, x, y, rad, rad, r, g, b, a);
}

void arcColor(Framebuffer& renderer, const short x, const short y, const short rad, const short start,
              const short end, const unsigned int color)
{
    arcRGBA(renderer, x, y, rad, start, end, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
            (color >> 24) & 0xFF);
}

void arcRGBA(Framebuffer& renderer, const short x, const short y, const short rad, short start, short end,
             const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    int stopValStart = 0, stopValEnd = 0;
    short cx = 0, cy = rad, df = static_cast<short>(1 - rad), d_e = 3, d_se = static_cast<short>(-2 * rad + 5);
    double temp = 0.0;

    if (rad < 0) return Helpers::warn("Unable to draw arc with negative radius");
    if (rad == 0) pixelRGBA(renderer, x, y, r, g, b, a);

    unsigned char drawOctant = 0;
    start %= 360;
    end %= 360;

    while (start < 0) start += 360;
    while (end < 0) end += 360;

    start %= 360;
    end %= 360;
    const int startOct = start / 45;
    const int endOct = end / 45;
    int oct = startOct % 8;

    while (oct != endOct)
    {
        if (oct == startOct)
        {
            const auto dStart = static_cast<double>(start);
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
            oct % 2 ? drawOctant |= 1 << oct : drawOctant &= 255 - (1 << oct);
        }

        if (oct == endOct)
        {
            const auto dEnd = static_cast<double>(end);
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
            else drawOctant |= 1 << oct;
        }
        else if (oct != startOct) drawOctant |= 1 << oct;

        oct = (oct + 1) % 8;
    }

    while (cx <= cy)
    {
        const auto yPcy = static_cast<short>(y + cy);
        const auto yMcy = static_cast<short>(y - cy);

        if (cx > 0)
        {
            const auto xPcx = static_cast<short>(x + cx);
            const auto xMcx = static_cast<short>(x - cx);

            if (drawOctant & 4) Helpers::pixel(renderer, xMcx, yPcy, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 2) Helpers::pixel(renderer, xPcx, yPcy, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 32) Helpers::pixel(renderer, xMcx, yMcy, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 64) Helpers::pixel(renderer, xPcx, yMcy, (a << 24) | (r << 16) | (g << 8) | b);
        }
        else
        {
            if (drawOctant & 96) Helpers::pixel(renderer, x, yMcy, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 6) Helpers::pixel(renderer, x, yPcy, (a << 24) | (r << 16) | (g << 8) | b);
        }

        const auto xPcy = static_cast<short>(x + cy);
        const auto xMcy = static_cast<short>(x - cy);

        if (cx > 0 && cx != cy)
        {
            const auto yPcx = static_cast<short>(y + cx);
            const auto yMcx = static_cast<short>(y - cx);

            if (drawOctant & 8) Helpers::pixel(renderer, xMcy, yPcx, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 1) Helpers::pixel(renderer, xPcy, yPcx, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 16) Helpers::pixel(renderer, xMcy, yMcx, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 128) Helpers::pixel(renderer, xPcy, yMcx, (a << 24) | (r << 16) | (g << 8) | b);
        }
        else if (cx == 0)
        {
            if (drawOctant & 24) Helpers::pixel(renderer, xMcy, y, (a << 24) | (r << 16) | (g << 8) | b);
            if (drawOctant & 129) Helpers::pixel(renderer, xPcy, y, (a << 24) | (r << 16) | (g << 8) | b);
        }

        if (stopValStart == cx)
            drawOctant & 1 << startOct
                ? drawOctant &= 255 - (1 << startOct)
                : drawOctant |= 1 << startOct;
        if (stopValEnd == cx)
            drawOctant & 1 << endOct
                ? drawOctant &= 255 - (1 << endOct)
                : drawOctant |= 1 << endOct;

        if (df < 0)
        {
            df = static_cast<short>(df + d_e);
            d_e += 2;
            d_se += 2;
        }
        else
        {
            df = static_cast<short>(df + d_se);
            d_e += 2;
            d_se += 4;
            cy--;
        }

        cx++;
    }
}

void aacircleColor(Framebuffer& renderer, const short x, const short y, const short rad, const unsigned int color)
{
    aaellipseRGBA(renderer, x, y, rad, rad, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                  (color >> 24) & 0xFF);
}

void aacircleRGBA(Framebuffer& renderer, const short x, const short y, const short rad, const unsigned char r,
                  const unsigned char g, const unsigned char b, const unsigned char a)
{
    aaellipseRGBA(renderer, x, y, rad, rad, r, g, b, a);
}

void ellipseColor(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                  const unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                         (color >> 24) & 0xFF, false);
}

void ellipseRGBA(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                 const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a, false);
}

void filledCircleColor(Framebuffer& renderer, const short x, const short y, const short r,
                       const unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, r, r, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                         (color >> 24) & 0xFF, true);
}

void filledCircleRGBA(Framebuffer& renderer, const short x, const short y, const short rad, const unsigned char r,
                      const unsigned char g, const unsigned char b, const unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rad, rad, r, g, b, a, true);
}

#if defined(_MSC_VER)
#   ifdef _M_X64
#   include <emmintrin.h>

static __inline long lrint(double d)
{
    const auto f = static_cast<float>(d);
    return _mm_cvtss_si32(_mm_load_ss(&f));
}

#   elif defined(_M_IX86)

__inline long int lrint(double flt)
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

__declspec(naked) long int lrint(double flt)
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

void aaellipseColor(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                    const unsigned int color)
{
    aaellipseRGBA(renderer, x, y, rx, ry, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                  (color >> 24) & 0xFF);
}

void aaellipseRGBA(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                   const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    int i;
    short xs, ys, xx, yy;
    double cp;
    unsigned char weight, iWeight;

    if (rx < 0 || ry < 0) return Helpers::warn("Unable to draw ellipse with negative radius");
    if (rx == 0)
        ry == 0
            ? pixelRGBA(renderer, x, y, r, g, b, a)
            : vlineRGBA(renderer, x, static_cast<short>(y - ry), static_cast<short>(y + ry), r, g, b, a);
    else if (ry == 0) hlineRGBA(renderer, static_cast<short>(x - rx), static_cast<short>(x + rx), y, r, g, b, a);

    const int a2 = rx * rx;
    const int b2 = ry * ry;
    const int ds = 2 * a2;
    const int dt = 2 * b2;
    const auto xc2 = static_cast<short>(2 * x);
    const auto yc2 = static_cast<short>(2 * y);

    const double root = sqrt(static_cast<double>(a2 + b2));
    const auto overdraw = static_cast<short>(lrint(root * 0.01) + 1);
    const int dxt = static_cast<short>(lrint(a2 / root) + overdraw);

    int t = 0;
    int s = -2 * a2 * ry;
    int d = 0;
    short xp = x;
    auto yp = static_cast<short>(y - ry);

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
            cp = static_cast<float>(abs(d)) / static_cast<float>(abs(s));
            if (cp > 1.0) cp = 1.0;
        }
        else cp = 1.0;

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

    const auto dyt = static_cast<short>(lrint(b2 / root) + overdraw);
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
        }
        else
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
        }
        else cp = 1.0;

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

void filledEllipseColor(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                        const unsigned int color)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                         (color >> 24) & 0xFF, true);
}

void filledEllipseRGBA(Framebuffer& renderer, const short x, const short y, const short rx, const short ry,
                       const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    Helpers::ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a, true);
}

void pieColor(Framebuffer& renderer, const short x, const short y, const short rad, const short start,
              const short end, const unsigned int color)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                     (color >> 24) & 0xFF, false);
}

void pieRGBA(Framebuffer& renderer, const short x, const short y, const short rad, const short start,
             const short end, const unsigned char r, const unsigned char g, const unsigned char b,
             const unsigned char a) { Helpers::pieRGBA(renderer, x, y, rad, start, end, r, g, b, a, false); }

void filledPieColor(Framebuffer& renderer, const short x, const short y, const short rad, const short start,
                    const short end, const unsigned int color)
{
    Helpers::pieRGBA(renderer, x, y, rad, start, end, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                     (color >> 24) & 0xFF, true);
}

void filledPieRGBA(Framebuffer& renderer, const short x, const short y, const short rad, const short start,
                   const short end, const unsigned char r, const unsigned char g, const unsigned char b,
                   const unsigned char a) { Helpers::pieRGBA(renderer, x, y, rad, start, end, r, g, b, a, true); }

void trigonColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                 const short x3, const short y3, const unsigned int color)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    polygonColor(renderer, vx, vy, 3, color);
}

void trigonRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                const short x3, const short y3, const unsigned char r, const unsigned char g, const unsigned char b,
                const unsigned char a)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    polygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void aatrigonColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                   const short x3, const short y3, const unsigned int color)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    aapolygonColor(renderer, vx, vy, 3, color);
}

void aatrigonRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                  const short x3, const short y3, const unsigned char r, const unsigned char g, const unsigned char b,
                  const unsigned char a)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    aapolygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void filledTrigonColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                       const short x3, const short y3, const unsigned int color)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    filledPolygonColor(renderer, vx, vy, 3, color);
}

void filledTrigonRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                      const short x3, const short y3, const unsigned char r, const unsigned char g,
                      const unsigned char b, const unsigned char a)
{
    const short vx[3] = {x1, x2, x3}, vy[3] = {y1, y2, y3};
    filledPolygonRGBA(renderer, vx, vy, 3, r, g, b, a);
}

void polygonColor(Framebuffer& renderer, const short* vx, const short* vy, const int n, const unsigned int color)
{
    polygonRGBA(renderer, vx, vy, n, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void polygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, const int n, const unsigned char r,
                 const unsigned char g, const unsigned char b, const unsigned char a)
{
    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");

    Helpers::polygon(renderer, vx, vy, n, (a << 24) | (r << 16) | (g << 8) | b);
}

void aapolygonColor(Framebuffer& renderer, const short* vx, const short* vy, const int n,
                    const unsigned int color)
{
    aapolygonRGBA(renderer, vx, vy, n, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void aapolygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, const int n, const unsigned char r,
                   const unsigned char g, const unsigned char b, const unsigned char a)
{
    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");
    for (int i = 0; i < n; ++i)
    {
        const int j = (i + 1) % n;
        Helpers::aalineRGBA(renderer, vx[i], vy[i], vx[j], vy[j], r, g, b, a, false);
    }
}

void filledPolygonRGBAMT(Framebuffer& renderer, const short* vx, const short* vy, const int n,
                         const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a,
                         int** polyInts, int* polyAllocated)
{
    int i, x1, x2, *polygonInts = nullptr, polygonAllocated = 0;
    if (vx == nullptr || vy == nullptr || n < 3) return Helpers::warn("Unable to draw polygon with less than 3 points");

    if (polyInts != nullptr && polyAllocated != nullptr)
    {
        polygonInts = *polyInts;
        polygonAllocated = *polyAllocated;
    }

    if (!polygonAllocated)
    {
        polygonInts = static_cast<int*>(malloc(sizeof(int) * n));
        polygonAllocated = n;
    }
    else if (polygonAllocated < n)
    {
        int* polygonIntsNew = static_cast<int*>(realloc(polygonInts, sizeof(int) * n));
        if (!polygonIntsNew)
        {
            if (!polygonInts)
            {
                free(polygonInts);
                polygonInts = nullptr;
            }

            polygonAllocated = 0;
        }
        else
        {
            polygonInts = polygonIntsNew;
            polygonAllocated = n;
        }
    }

    if (polygonInts == nullptr) polygonAllocated = 0;
    if (polyInts != nullptr) *polyInts = polygonInts;
    if (polyAllocated != nullptr) *polyAllocated = polygonAllocated;
    if (polygonInts == nullptr) return Helpers::warn("Unable to allocate space for polygon fill");

    int miny = vy[0];
    int maxy = vy[0];

    for (i = 1; i < n; ++i)
        if (vy[i] < miny) miny = vy[i];
        else if (vy[i] > maxy) maxy = vy[i];

    for (int y = miny; y <= maxy; ++y)
    {
        int ints = 0;
        for (i = 0; i < n; ++i)
        {
            const int ind1 = i ? i - 1 : n - 1;
            const int ind2 = i ? i : 0;
            int y1 = vy[ind1];
            int y2 = vy[ind2];

            if (y1 < y2)
            {
                x1 = vx[ind1];
                x2 = vx[ind2];
            }
            else if (y1 > y2)
            {
                y2 = vy[ind1];
                y1 = vy[ind2];
                x2 = vx[ind1];
                x1 = vx[ind2];
            }
            else continue;

            if ((y >= y1 && y < y2) || (y == maxy && y > y1 && y <= y2))
                polygonInts[ints++] = 65536 * (y - y1) / (y2
                    - y1) * (x2 - x1) + 65536 * x1;
        }

        qsort(polygonInts, ints, sizeof(int), Helpers::compareInt);

        for (i = 0; i < ints; i += 2)
        {
            int xa = polygonInts[i] + 1;
            xa = (xa >> 16) + ((xa & 32768) >> 15);
            int xb = polygonInts[i + 1] - 1;
            xb = (xb >> 16) + ((xb & 32768) >> 15);

            hlineRGBA(renderer, static_cast<short>(xa), static_cast<short>(xb), static_cast<short>(y), r, g, b, a);
        }
    }
}

void filledPolygonColor(Framebuffer& renderer, const short* vx, const short* vy, const int n,
                        const unsigned int color)
{
    filledPolygonRGBAMT(renderer, vx, vy, n, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                        (color >> 24) & 0xFF, nullptr, nullptr);
}

void filledPolygonRGBA(Framebuffer& renderer, const short* vx, const short* vy, const int n,
                       const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
    filledPolygonRGBAMT(renderer, vx, vy, n, r, g, b, a, nullptr, nullptr);
}

void bezierColor(Framebuffer& renderer, const short* vx, const short* vy, const int n, const int s,
                 const unsigned int color)
{
    bezierRGBA(renderer, vx, vy, n, s, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF, (color >> 24) & 0xFF);
}

void bezierRGBA(Framebuffer& renderer, const short* vx, const short* vy, const int n, const int s,
                const unsigned char r,
                const unsigned char g, const unsigned char b, const unsigned char a)
{
    int i;

    if (n < 3 || s < 2) return Helpers::warn("Unable to draw bezier curve with less than 3 points or 2 subdivisions");
    const double stepSize = 1.0 / s;

    const auto x = static_cast<double*>(malloc(sizeof(double) * (n + 1)));
    const auto y = static_cast<double*>(malloc(sizeof(double) * (n + 1)));

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

    double t = 0.0;
    auto x1 = static_cast<short>(lrint(Helpers::evaluateBezier(x, n + 1, t)));
    auto y1 = static_cast<short>(lrint(Helpers::evaluateBezier(y, n + 1, t)));

    for (i = 0; i <= n * s; ++i)
    {
        t += stepSize;
        const auto x2 = static_cast<short>(Helpers::evaluateBezier(x, n, t));
        const auto y2 = static_cast<short>(Helpers::evaluateBezier(y, n, t));
        Line(renderer, x1, y1, x2, y2).draw((a << 24) | (r << 16) | (g << 8) | b);

        x1 = x2;
        y1 = y2;
    }

    free(x);
    free(y);
}

void thickLineColor(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                    const unsigned char width, const unsigned int color)
{
    thickLineRGBA(renderer, x1, y1, x2, y2, width, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color) & 0xFF,
                  (color >> 24) & 0xFF);
}

void thickLineRGBA(Framebuffer& renderer, const short x1, const short y1, const short x2, const short y2,
                   const unsigned char width, const unsigned char r, const unsigned char g, const unsigned char b,
                   const unsigned char a)
{
    if (width < 1) return Helpers::warn("Unable to draw thick line with width less than 1");

    if (x1 == x2 && y1 == y2)
    {
        const int wh = width / 2;
        boxRGBA(renderer, static_cast<short>(x1 - wh), static_cast<short>(y1 - wh), static_cast<short>(x2 + width),
                static_cast<short>(y2 + width), r, g, b, a);
    }

    if (width == 1) lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);

    const auto dx = static_cast<double>(x2 - x1);
    const auto dy = static_cast<double>(y2 - y1);

    const double l = sqrt(dx * dx + dy * dy);
    const double ang = atan2(dx, dy);
    const double adj = 0.1 + 0.9 * fabs(cos(2.0 * ang));
    const double wl2 = (width - adj) / (2.0 * l);
    const double nx = dx * wl2;
    const double ny = dy * wl2;

    const auto dx1 = static_cast<double>(x1);
    const auto dy1 = static_cast<double>(y1);
    const auto dx2 = static_cast<double>(x2);
    const auto dy2 = static_cast<double>(y2);

    const short px[4] = {
        static_cast<short>(dx1 + ny), static_cast<short>(dx1 - ny), static_cast<short>(dx2 - ny),
        static_cast<short>(dx2 + ny)
    };
    const short py[4] = {
        static_cast<short>(dy1 - nx), static_cast<short>(dy1 + nx), static_cast<short>(dy2 + nx),
        static_cast<short>(dy2 - nx)
    };

    filledPolygonRGBA(renderer, px, py, 4, r, g, b, a);
}
