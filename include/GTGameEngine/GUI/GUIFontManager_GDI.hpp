// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GUI_FontManager_GDI
#define GT_GUI_FontManager_GDI

#include "../Config.hpp"

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#include <GTLib/HandleManager.hpp>
#include <GTLib/Colour.hpp>
#include "GUIFontManager.hpp"

namespace GT
{
    /// GDI implementation
    class GUIFontManager_GDI : public GUIFontManager
    {
    public:

        /// Constructor.
        GUIFontManager_GDI(uint32_t options = 0);

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

        /// @copydoc GUIFontManager::GetTextCursorPositionFromPoint()
        bool GetTextCursorPositionFromPoint(HGUIFont hFont, const char* text, unsigned int textLengthInChars, unsigned int maxWidth, int inputPosX, int &textCursorPosXOut, unsigned int &characterIndexOut) const;

        /// @copydoc GUIFontManager::GetTextCursorPositionFromPoint()
        bool GetTextCursorPositionFromCharacter(HGUIFont hFont, const char* text, unsigned int characterIndex, int &textCursorPosXOut) const;

        /// @copydoc GUIFontManager::DrawTextToBuffer()
        bool DrawTextToBuffer(HGUIFont hFont, const char* text, size_t textLengthInChars, GTLib::Colour color, void* bufferOut, size_t bufferOutSize);


        /// Retrieves the Win32 font handle from the given font.
        ///
        /// @param hFont [in] The font to retrieve the Win32 handle from.
        ///
        /// @return The Win32 font if any, or 0 if an error occurs or the font handle is invalid.
        HFONT GetWin32FontHandle(HGUIFont hFont);


    private:

        /// A handle to the device context for doing text rendering operations.
        HDC m_hDC;

        /// The bitmap to draw text into.
        HBITMAP m_hTextBitmap;

        /// A pointer to the buffer containing the bitmap data.
        void* m_pTextBitmapData;

        /// The size of the text bitmap. We use this to determine if it needs to be inflated.
        unsigned int m_textBitmapWidth;
        unsigned int m_textBitmapHeight;


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
