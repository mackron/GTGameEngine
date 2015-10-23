// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GUI/ClippingVolume.hpp>

namespace GTGUI
{
    ClippingVolume::ClippingVolume(float left, float top, float right, float bottom)
        : left(left), top(top), right(right), bottom(bottom)
    {
    }

    ClippingVolume::~ClippingVolume()
    {
    }
}