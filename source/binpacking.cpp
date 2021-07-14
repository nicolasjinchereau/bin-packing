/*---------------------------------------------------------------------------------------------
*  Copyright (c) 2020 Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <BinPacking.h>
#include <vector>
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <cassert>

using namespace std;

namespace binpacking
{

Bin BinPacker::PackBin(
    std::vector<RectMapping>& input,
    const std::vector<Size>& binSizes,
    int padding,
    bool allowRotation,
    std::vector<RectMapping>& overflow)
{
    for(size_t i = 0; i < binComparisons.size(); ++i)
    {
        sortedInput[i] = input;
        sort(sortedInput[i].begin(), sortedInput[i].end(), binComparisons[i]);
    }

    int largestArea = 0;
    int bestOrderIndex = -1;
    int bestSize = 0;
    int accepted = 0;
    int remaining = 0;

    NodePtr root = nodeAllocator->GetNode();

    for(size_t i = 0; i < binComparisons.size(); ++i)
    {
        int binSizeCount = (int)binSizes.size();
        for(int size = bestSize; size < binSizeCount; ++size)
        {
            int area = 0;

            Size sz = binSizes[size];
            root->Reset(Rect(0, 0, sz.x, sz.y));

            int acc = 0;
            int rem = 0;

            for(auto& loc : sortedInput[i])
            {
                if(root->Insert(loc, padding, allowRotation)) {
                    area += loc.mappedRect.area();
                    ++acc;
                }
                else {
                    ++rem;
                }
            }

            if((area > largestArea && size >= bestSize)
            || (size > bestSize && area >= largestArea))
            {
                largestArea = area;
                bestOrderIndex = i;
                bestSize = size;
                accepted = acc;
                remaining = rem;
            }
            else
            {
                break;
            }
        }
    }

    Bin bin;
    bin.size = binSizes[bestSize];
    bin.mappings;
    overflow.reserve(remaining);

    root->Reset(Rect(bin.size));

    for(auto& loc : sortedInput[bestOrderIndex])
    {
        auto node = root->Insert(loc, padding, allowRotation);
        if (node) {
            bin.mappings.push_back(loc);
            node->pMapping = &bin.mappings.back();
        }
        else {
            overflow.push_back(loc);
        }
    }

    return bin;
}

void BinPacker::PackBoxes(const std::vector<Size>& boxes, int maxSize, int padding, bool allowRotation)
{
    if(maxSize > 0 && (maxSize & (maxSize - 1)) != 0)
        throw std::runtime_error("'maxSize' must be a power of two");

    dynamicPacking = false;

    input.clear();
    overflow.clear();
    
    input.reserve(boxes.size());
    int inputIndex = 0;

    for(auto& box : boxes)
    {
        if(box.x > maxSize || box.y > maxSize)
            throw std::runtime_error("all boxes must fit inside bounds 'maxSize'x'maxSize'");

        input.push_back(RectMapping(box, inputIndex++));
    }
    
    binSizes.clear();
    binSizes.reserve((int)(log2(maxSize) * 3 + 0.5));

    for(int binSize = maxSize; binSize > 1; binSize /= 2)
    {
        binSizes.push_back(Size(binSize, binSize));
        binSizes.push_back(Size(binSize, binSize / 2));
        binSizes.push_back(Size(binSize / 2, binSize));
    }

    bins.clear();
    bins.reserve(4);

    while(true)
    {
        Bin bin = PackBin(input, binSizes, padding, allowRotation, overflow);
        bins.emplace_back(move(bin));

        if(overflow.empty())
            break;

        overflow.swap(input);
        overflow.clear();
    }
}

void BinPacker::StartDynamicPacking(int binSize, int boxPadding, bool allowRotation)
{
    dynamicPacking = true;
    this->binSize = binSize;
    this->boxPadding = boxPadding;
    this->allowRotation = allowRotation;

    bins.clear();

    Bin bin({ binSize, binSize });
    bin.root = nodeAllocator->GetNode();
    bin.root->Reset(Rect(0, 0, binSize, binSize));
    bins.push_back(std::move(bin));
}

RectMapping BinPacker::PackBox(const Size& box)
{
    if (!dynamicPacking)
        throw std::runtime_error("'StartDynamicPacking' must be called first");

    if(box.x > binSize || box.y > binSize)
        throw std::runtime_error("box is too large");
    
    Node* insertedNode = nullptr;
    
    int i = 0;

    for ( ; i < (int)bins.size(); ++i)
    {
        auto& bin = bins[i];
        auto mapping = RectMapping{ box, i };

        insertedNode = bin.root->Insert(mapping, boxPadding, allowRotation);
        if (insertedNode)
        {
            bin.mappings.push_back(mapping);
            insertedNode->pMapping = &bin.mappings.back();
            break;
        }
    }

    if (!insertedNode)
    {
        Bin newBin({ binSize, binSize });
        newBin.root = nodeAllocator->GetNode();
        newBin.root->Reset(Rect(0, 0, binSize, binSize));
        bins.push_back(std::move(newBin));

        auto& bin = bins.back();
        auto mapping = RectMapping{ box, i };

        insertedNode = bin.root->Insert(mapping, boxPadding, allowRotation);
        assert(insertedNode);

        bin.mappings.push_back(mapping);
        insertedNode->pMapping = &bin.mappings.back();
    }

    return *insertedNode->pMapping;
}

}
