// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/ClippingVolume.hpp>

namespace GTGUI
{
    GUIClippingVolume::GUIClippingVolume(float left, float top, float right, float bottom)
        : left(left), top(top), right(right), bottom(bottom)
    {
    }

    GUIClippingVolume::~GUIClippingVolume()
    {
    }
}