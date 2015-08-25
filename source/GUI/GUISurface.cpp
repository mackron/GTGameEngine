// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUISurface.hpp>

namespace GT
{
    GUISurface::GUISurface()
        : id(),
          width(0),
          height(0),
          xDPI(96),
          yDPI(96),
          topLevelElements(),
          auxData(nullptr),
          invalidRect()
    {
    }

    GUISurface::~GUISurface()
    {
    }
}
