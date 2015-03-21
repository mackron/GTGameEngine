// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIElement.hpp>

namespace GT
{
    GUIElement::GUIElement(HGUIElement handle)
        : handle(handle),
          id(),
          layout(),
          style(),
          parent(0),
          firstChild(0), lastChild(0),
          prevSibling(0), nextSibling(0),
          textManager(nullptr),
          eventHandlers()
    {
    }

    GUIElement::~GUIElement()
    {
    }
}