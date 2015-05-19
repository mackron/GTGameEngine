// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUISurface.hpp>

namespace GT
{
    GUISurface::GUISurface(HGUISurface handle)
        : handle(handle),
          id(),
          width(0),
          height(0),
          xDPI(96),
          yDPI(96),
          topLevelElements(),
          auxData(nullptr),
          hElementCapturingMouseEvents(0),
          hElementUnderMouse(0),
          paintingMode(GUIPaintingMode::Immediate),
          invalidRect()
    {
    }

    GUISurface::~GUISurface()
    {
    }
}