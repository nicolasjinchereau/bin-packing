/*---------------------------------------------------------------------------------------------
*  Copyright (c) 2020 Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <list>
#include <Size.h>
#include <Rect.h>
#include <Bin.h>
#include <Node.h>
#include <NodeAllocator.h>

namespace binpacking
{

class BinPacker
{
    static bool IsAreaDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.area() > b.inputSize.area();
    }

    static bool IsPerimeterDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.perimeter() > b.inputSize.perimeter();
    }

    static bool IsMaxSideLengthDescending(const RectMapping& a, const RectMapping& b) {
        return std::max(a.inputSize.x, a.inputSize.y) > std::max(b.inputSize.x, b.inputSize.y);
    }

    static bool IsMaxWidthDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.x > b.inputSize.x;
    }

    static bool IsMaxHeightDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.y > b.inputSize.y;
    }

    typedef bool (*BinComparison)(const RectMapping& a, const RectMapping& b);

    constexpr static int NumBinComparison = 5;

    std::array<BinComparison, NumBinComparison> binComparisons {
        IsAreaDescending,
        IsPerimeterDescending,
        IsMaxSideLengthDescending,
        IsMaxWidthDescending,
        IsMaxHeightDescending
    };

    std::array<std::vector<RectMapping>, NumBinComparison> sortedInput;
    std::vector<RectMapping> input;
    std::vector<RectMapping> overflow;
    std::vector<Size> binSizes;
    std::vector<Bin> bins;
    
    std::shared_ptr<NodeAllocator> nodeAllocator = std::make_shared<NodeAllocator>();

    bool dynamicPacking = false;
    int binSize = 0;
    int boxPadding = 0;
    bool allowRotation = true;

    Bin PackBin(
        std::vector<RectMapping>& input,
        const std::vector<Size>& binSizes,
        int padding, bool allowRotation,
        std::vector<RectMapping>& overflow);

public:
    void PackBoxes(
        const std::vector<Size>& boxes,
        int maxSize,
        int padding,
        bool allowRotation = true);

    void StartDynamicPacking(int binSize, int boxPadding, bool allowRotation);
    RectMapping PackBox(const Size& box);

    const std::vector<Bin>& GetBins() const {
        return bins;
    }
};

}
