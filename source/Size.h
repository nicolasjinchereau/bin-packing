#pragma once

namespace binpacking
{

struct Size
{
    int x, y;

    Size(int x = 0, int y = 0)
        : x(x), y(y) {}

    int area() const {
        return x * y;
    }

    int perimeter() const {
        return (x + y) * 2;
    }
};

}
