// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GUI_FontManager_GDI_hpp_
#define __GT_GUI_FontManager_GDI_hpp_

#include "../../Config.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTLib/HandleManager.hpp>
#include "GUIFontManager.hpp"

namespace GT
{
    /// GDI implementation 
    class GUIFontManager_GDI : public GUIFontManager
    {
    public:

        /// Constructor.
        GUIFontManager_GDI();

        /// Destructor.
        ~GUIFontManager_GDI();


        /// @copydoc GUIFontManager::CreateFont()
        HGUIFont CreateFont(const GUIFontInfo &fontInfo);

        /// @copydoc GUIFontManager::DeleteFont()
        void DeleteFont(HGUIFont hFont);

        /// @copydoc GUIFontManager::MeasureString()
        bool MeasureString(HGUIFont hFont, const char* text, size_t textLengthInChars, int &widthOut, int &heightOut);

        /// @copydoc GUIFontManager::GetAscent()
        int GetAscent(HGUIFont hFont) const;

        /// @copydoc GUIFontManager::GetDescent()
        int GetDescent(HGUIFont hFont) const;

        /// @copydoc GUIFontManager::GetLineHeight()
        int GetLineHeight(HGUIFont hFont) const;

        /// @copydoc GUIFontManager::GetGlyphMetrics()
        bool GetGlyphMetrics(HGUIFont hFont, char32_t character, GUIGlyphMetrics &metricsOut) const;


        /// Retrieves the Win32 font handle from the given font.
        ///
        /// @param hFont [in] The font to retrieve the Win32 handle from.
        ///
        /// @return The Win32 font if any, or 0 if an error occurs or the font handle is invalid.
        HFONT GetWin32FontHandle(HGUIFont hFont);


    private:

        /// A handle to the device context for doing text rendering operations.
        HDC m_hDC;


        /// Structure containing information about a GDI font.
        struct FontGDI
        {
            FontGDI(HFONT hFontWin32In, TEXTMETRICA metricsIn)
                : hFontWin32(hFontWin32In), metrics(metricsIn)
            {
            }

            /// A handle to the Win32 font.
            HFONT hFontWin32;

            /// The font metrics.
            TEXTMETRICA metrics;
        };

        /// Tha manager for font handles.
        HandleManager<HGUIFont, FontGDI> m_fontHandles;
    };
}

#endif

#endif
