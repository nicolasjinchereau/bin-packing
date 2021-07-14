#pragma once
#include <Size.h>

namespace binpacking
{

struct Rect
{
    int x, y, w, h;

    Rect()
        : x(0), y(0), w(0), h(0) {}

    Rect(int x, int y, int width, int height)
        : x(x), y(y), w(width), h(height) {}

    Rect(Size size)
        : x(0), y(0), w(size.x), h(size.y) {}

    int area() const {
        return w * h;
    }

    int perimeter() const {
        return (w + h) * 2;
    }
};

}
