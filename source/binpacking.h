/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <vector>

namespace binpacking
{
    struct Size
    {
        int x, y;

        Size(int x = 0, int y = 0)
            : x(x), y(y){}

        int area() const {
            return x * y;
        }

        int perimeter() const {
            return (x + y) * 2;
        }
    };

    struct Rect
    {
        int x, y, w, h;

        Rect()
            : x(0), y(0), w(0), h(0){}

        Rect(int x, int y, int width, int height)
            : x(x), y(y), w(width), h(height){}

        Rect(Size size)
            : x(0), y(0), w(size.x), h(size.y){}

        int area() const {
            return w * h;
        }

        int perimeter() const {
            return (w + h) * 2;
        }
    };
    
    struct RectMapping
    {
        int inputIndex = 0;
        Size inputSize;
        Rect mappedRect;
        bool rotated = false;
        
        RectMapping(){}
        RectMapping(const Size& inputSize, int inputIndex)
            : inputSize(inputSize), mappedRect(inputSize), inputIndex(inputIndex){}
    };

    struct Bin
    {
        Size size;
        std::vector<RectMapping> mappings;

        Bin(){}
        
        Bin(Bin&& bin) :
            mappings(move(bin.mappings)), size(bin.size)
        {
            bin.size = Size();
        }

        Bin& operator=(Bin&& bin)
        {
            mappings = move(bin.mappings);
            size = bin.size;
            bin.size = Size();
        }

        Bin(const Bin&) = default;
        Bin& operator=(const Bin&) = default;
    };

    std::vector<Bin> Pack(
        const std::vector<Size>& sizes,
        int maxSize,
        int padding, 
        bool allowRotation = true);
}
