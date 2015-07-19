// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorTheme_hpp_
#define __GT_EditorTheme_hpp_

#include <GTLib/Colour.hpp>

namespace GT
{
    /// Structure containing theme information for the editor.
    struct EditorTheme
    {
        EditorTheme()
            : backgroundDark(0.225f, 0.225f, 0.225f),
              backgroundMid(0.25f, 0.25f, 0.25f),
              backgroundPopup(0.2f, 0.2f, 0.2f),

              borderDefault(0.4f, 0.4f, 0.4f)
        {
        }


        // Background colours.
        GTLib::Colour backgroundDark;       //< The dark background colour.
        GTLib::Colour backgroundMid;        //< The mid tone background colour.
        GTLib::Colour backgroundPopup;      //< The background colour to use for popups.

        // Border colours.
        GTLib::Colour borderDefault;        //< The default border colours.
    };
}

#endif
