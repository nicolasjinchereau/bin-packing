/*---------------------------------------------------------------------------------------------
*  Copyright (c) 2020 Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <vector>
#include <list>
#include <Size.h>
#include <Rect.h>
#include <RectMapping.h>
#include <NodeAllocator.h>
#include <Node.h>

namespace binpacking
{

struct Bin
{
    Size size;
    NodePtr root;
    std::list<RectMapping> mappings;

    Bin(){}
    Bin(const Size& size) : size(size){}

    Bin(Bin&& bin) noexcept
        : size(bin.size), root(std::move(bin.root)), mappings(move(bin.mappings))
    {
        bin.size = Size();
    }

    Bin& operator=(Bin&& bin) noexcept
    {
        size = bin.size;
        bin.size = Size();
        root = std::move(bin.root);
        mappings = move(bin.mappings);
        return *this;
    }

    Bin(const Bin&) = default;
    Bin& operator=(const Bin&) = default;
};

}
