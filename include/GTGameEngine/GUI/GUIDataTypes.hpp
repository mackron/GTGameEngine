// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_HandleTypes
#define GT_GUI_HandleTypes

#include <cstdint>

namespace GT
{
    /// Basic type representing a handle to an element.
    typedef uint32_t HGUIElement;

    /// Basic type representing a handle to an element.
    typedef uint32_t HGUISurface;


    /// Enumerator for the various painting modes.
    enum class GUIPaintingMode
    {
        Immediate,          ///< Paint commands are posted as soon as a graphical change is made.
        Deferred            ///< Paint commands are posted via an explicit command to GUIContext::PaintSurface().
    };
}

#endif
