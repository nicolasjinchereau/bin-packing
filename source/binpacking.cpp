/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <binpacking.h>
#include <vector>
#include <algorithm>
#include <array>
#include <memory>
#include <stdexcept>
using namespace std;

namespace binpacking
{
    bool IsAreaDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.area() > b.inputSize.area();
    }

    bool IsPerimeterDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.perimeter() > b.inputSize.perimeter();
    }

    bool IsMaxSideLengthDescending(const RectMapping& a, const RectMapping& b) {
        return max(a.inputSize.x, a.inputSize.y) > max(b.inputSize.x, b.inputSize.y);
    }

    bool IsMaxWidthDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.x > b.inputSize.x;
    }

    bool IsMaxHeightDescending(const RectMapping& a, const RectMapping& b) {
        return a.inputSize.y > b.inputSize.y;
    }

    typedef bool (*BinComparison)(const RectMapping& a, const RectMapping& b);

    array<BinComparison, 5> binComparisons {
        IsAreaDescending,
        IsPerimeterDescending,
        IsMaxSideLengthDescending,
        IsMaxWidthDescending,
        IsMaxHeightDescending
    };

    enum class NodeType
    {
        Empty,   // no contents, no children
        Branch,  // has contents, has children
        Leaf     // has contents, no children
    };

    struct Node
    {
        Rect rect = Rect();
        NodeType type = NodeType::Empty;
        RectMapping* pMapping = nullptr;
        unique_ptr<Node> left;
        unique_ptr<Node> right;
    
        void Reset(const Rect& rc)
        {
            rect = rc;
            pMapping = nullptr;
            type = NodeType::Empty;
        }

        Node* Insert(RectMapping& mapping, int padding, bool allowRotation)
        {
            if(type == NodeType::Empty)
            {
                if(mapping.inputSize.x == rect.w &&
                   mapping.inputSize.y == rect.h)
                {
                    pMapping = &mapping;
                    pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
                    pMapping->rotated = false;
                    type = NodeType::Leaf;
                    return this;
                }
                else if(allowRotation &&
                        mapping.inputSize.x == rect.h &&
                        mapping.inputSize.y == rect.w)
                {
                    pMapping = &mapping;
                    pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
                    pMapping->rotated = true;
                    type = NodeType::Leaf;
                    return this;
                }
                else if(mapping.inputSize.x <= rect.w &&
                        mapping.inputSize.y <= rect.h)
                {
                    pMapping = &mapping;
                    pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
                    pMapping->rotated = false;
                    SplitBranch(padding);
                    type = NodeType::Branch;
                    return this;
                }
                else if(allowRotation &&
                        mapping.inputSize.x <= rect.h &&
                        mapping.inputSize.y <= rect.w)
                {
                    pMapping = &mapping;
                    pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
                    pMapping->rotated = true;
                    SplitBranch(padding);
                    type = NodeType::Branch;
                    return this;
                }
            }
            else if(type == NodeType::Branch)
            {
                auto ret = left->Insert(mapping, padding, allowRotation);
                if(!ret) ret = right->Insert(mapping, padding, allowRotation);
                return ret;
            }

            return nullptr;
        }

        void SplitBranch(int padding)
        {
            if(!left) left = make_unique<Node>();
            if(!right) right = make_unique<Node>();
            
            int remWidth = rect.w - pMapping->mappedRect.w;
            int remHeight = rect.h - pMapping->mappedRect.h;

            if(remWidth > remHeight)
            {
                // split vertically
                left->rect = Rect(rect.x, rect.y + pMapping->mappedRect.h, pMapping->mappedRect.w, remHeight);
                left->type = NodeType::Empty;
                left->pMapping = nullptr;
                right->rect = Rect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, rect.h);
                right->type = NodeType::Empty;
                right->pMapping = nullptr;

                left->rect.y += padding;
                left->rect.h -= padding;
                right->rect.x += padding;
                right->rect.w -= padding;
            }
            else
            {
                // split horizontally
                left->rect = Rect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, pMapping->mappedRect.h);
                left->type = NodeType::Empty;
                left->pMapping = nullptr;
                right->rect = Rect(rect.x, rect.y + pMapping->mappedRect.h, rect.w, remHeight);
                right->type = NodeType::Empty;
                right->pMapping = nullptr;

                left->rect.x += padding;
                left->rect.w -= padding;
                right->rect.y += padding;
                right->rect.h -= padding;
            }
        }
    };

    Bin PackBin(vector<RectMapping>& input, const vector<Size>& binSizes, int padding, bool allowRotation, vector<RectMapping>& overflow)
    {
        array<vector<RectMapping>, binComparisons.size()> sortedInput;
        
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

        Node root;

        for(size_t i = 0; i < binComparisons.size(); ++i)
        {
            int binSizeCount = (int)binSizes.size();
            for(int size = bestSize; size < binSizeCount; ++size)
            {
                int area = 0;

                Size sz = binSizes[size];
                root.Reset(Rect(0, 0, sz.x, sz.y));

                int acc = 0;
                int rem = 0;

                for(auto& loc : sortedInput[i])
                {
                    if(root.Insert(loc, padding, allowRotation))
                    {
                        area += loc.mappedRect.area();
                        ++acc;
                    }
                    else
                    {
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
        bin.mappings.reserve(accepted);
        overflow.reserve(remaining);

        root.Reset(Rect(bin.size));

        for(auto& loc : sortedInput[bestOrderIndex])
        {
            if(root.Insert(loc, padding, allowRotation))
                bin.mappings.push_back(loc);
            else
                overflow.push_back(loc);
        }

        return bin;
    }

    vector<Bin> Pack(const vector<Size>& sizes, int maxSize, int padding, bool allowRotation)
    {
        if(maxSize > 0 && (maxSize & (maxSize - 1)) != 0)
            throw runtime_error("'maxSize' must be a power of two");

        vector<RectMapping> input;
        vector<RectMapping> overflow;
        
        input.reserve(sizes.size());
        int inputIndex = 0;

        for(auto& size : sizes)
        {
            if(size.x > maxSize || size.y > maxSize)
                throw runtime_error("all sizes must fit inside bounds 'maxSize'x'maxSize'");

            input.push_back(RectMapping(size, inputIndex++));
        }
        
        vector<Size> binSizes;
        binSizes.reserve((int)(log2(maxSize) * 3 + 0.5));

        for(int binSize = maxSize; binSize > 1; binSize /= 2)
        {
            binSizes.push_back(Size(binSize, binSize));
            binSizes.push_back(Size(binSize, binSize / 2));
            binSizes.push_back(Size(binSize / 2, binSize));
        }

        vector<Bin> bins;
        bins.reserve(8);

        while(true)
        {
            Bin bin = PackBin(input, binSizes, padding, allowRotation, overflow);
            bins.emplace_back(move(bin));

            if(overflow.empty())
                break;

            overflow.swap(input);
            overflow.clear();
        }

        return bins;
    }
}
