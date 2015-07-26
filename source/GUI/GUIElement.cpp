// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIElement.hpp>
#include <GTGameEngine/GUI/GUIFontManager.hpp>

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
          hFont(0),
          pTextLayout(nullptr),
          eventHandlers(), pCallbackEventHandlers(nullptr),
          flags(0)
    {
    }

    GUIElement::~GUIElement()
    {
    }
}