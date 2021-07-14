/*---------------------------------------------------------------------------------------------
*  Copyright (c) 2020 Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <Size.h>
#include <Rect.h>

namespace binpacking
{

struct RectMapping
{
    int inputIndex = 0;
    Size inputSize;
    Rect mappedRect;
    bool rotated = false;

    RectMapping() {}
    RectMapping(const Size& inputSize, int inputIndex)
        : inputSize(inputSize),
        mappedRect(inputSize),
        inputIndex(inputIndex) {}
};

}
