// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorTheme
#define GT_EditorTheme

#include "../Config.hpp"
#include <GTLib/String.hpp>
#include <GTLib/Color.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    /// Structure containing theme information for the editor.
    struct EditorTheme
    {
        EditorTheme()
            : defaultFontFamily(),
              defaultSymbolFontFamily(),
              defaultTextColor(0.8f, 0.8f, 0.8f),

              backgroundDark(0.225f, 0.225f, 0.225f),
              backgroundMid(0.25f, 0.25f, 0.25f),
              backgroundPopup(0.2f, 0.2f, 0.2f),

              borderDefault(0.4f, 0.4f, 0.4f),
              borderDim(0.3f, 0.3f, 0.3f),
              borderDark(0.2f, 0.2f, 0.2f),

              scrollbarBackgroundColor(0.3f, 0.3f, 0.3f),
              scrollbarWidth(16)
        {
#if defined(GT_PLATFORM_WINDOWS)
            GTLib::Strings::Copy(defaultFontFamily, "Segoe UI");
            GTLib::Strings::Copy(defaultSymbolFontFamily, "Segoe UI Symbol");
#endif
        }


        // Fonts and text
        char defaultFontFamily[128];
        char defaultSymbolFontFamily[128];
        GT::Color defaultTextColor;

        // Background colours.
        GT::Color backgroundDark;       ///< The dark background colour.
        GT::Color backgroundMid;        ///< The mid tone background colour.
        GT::Color backgroundPopup;      ///< The background colour to use for popups.

        // Border colours.
        GT::Color borderDefault;        ///< The default border colours.
        GT::Color borderDim;            ///< A border color that is slightly less bright than the default border.
        GT::Color borderDark;           ///< A border color that is darker than the default border.

        // Scroll bars.
        GT::Color scrollbarBackgroundColor;
        unsigned int scrollbarWidth;
    };
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
