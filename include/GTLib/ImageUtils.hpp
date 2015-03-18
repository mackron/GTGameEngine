// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_ImageUtils_hpp_
#define __GT_ImageUtils_hpp_

#include "Math.hpp"

namespace GT
{
    /// Calculates the size of a mipmap based on it's index and the base size.
    template <typename U, typename T>
    T CalculateMipmapSize(U mipmapIndex, T baseSize)
    {
        return static_cast<T>(GTLib::Max(1.0, baseSize / std::pow(2.0, static_cast<int>(mipmapIndex))));
    }

    /// Calculates the total number of mipmaps that would be created based on the given image dimensions.
    template <typename T>
    T CalculateMipmapCount(T baseWidth, T baseHeight, T baseDepth)
    {
        const float maxsize = static_cast<float>(GTLib::Max(GTLib::Max(baseWidth, baseHeight), baseDepth));
        return static_cast<unsigned int>((std::log(maxsize) / std::log(2.0f)) + 1);
    }

    template <typename T>
    T CalculateMipmapCount(T baseWidth, T baseHeight)
    {
        return CalculateMipmapCount(baseWidth, baseHeight, static_cast<T>(1));
    }

    template <typename T>
    T CalculateMipmapCount(T baseWidth)
    {
        return CalculateMipmapCount(baseWidth, static_cast<T>(1), static_cast<T>(1));
    }
}

#endif
