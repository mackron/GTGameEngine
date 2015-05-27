// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIElement.hpp>

namespace GT
{
    GUIElement::GUIElement()
        : pSurface(nullptr),
          id(),
          layout(),
          style(),

          pParent(nullptr),
          pFirstChild(nullptr), pLastChild(nullptr),
          pPrevSibling(nullptr), pNextSibling(nullptr),

          textManager(nullptr),
          eventHandlers()
    {
    }

    GUIElement::~GUIElement()
    {
    }
}