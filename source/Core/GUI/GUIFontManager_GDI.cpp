// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIFontManager_GDI.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/FontStyles.hpp>

namespace GT
{
    GUIFontManager_GDI::GUIFontManager_GDI()
        : GUIFontManager(),
          m_hDC(NULL)
    {
        m_hDC = CreateCompatibleDC(GetDC(GetDesktopWindow()));
		SetMapMode(m_hDC, MM_TEXT);
    }

    GUIFontManager_GDI::~GUIFontManager_GDI()
    {
        // All loaded fonts need to be deleted.
        this->DeleteAllFonts();
        

        // The device context should be deleted last because things like fonts can depend on it.
        DeleteDC(m_hDC);
    }


    HGUIFont GUIFontManager_GDI::CreateFont(const GUIFontInfo &fontInfo)
    {
        LONG weight = FW_REGULAR;
		if ((fontInfo.styleFlags & GTLib::FontStyle_Bold) != 0)
		{
			weight = FW_BOLD;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_Thin) != 0)
		{
			weight = FW_THIN;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_ExtraLight) != 0)
		{
			weight = FW_EXTRALIGHT;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_Light) != 0)
		{
			weight = FW_LIGHT;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_Medium) != 0)
		{
			weight = FW_MEDIUM;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_SemiBold) != 0)
		{
			weight = FW_SEMIBOLD;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_ExtraBold) != 0)
		{
			weight = FW_EXTRABOLD;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_Heavy) != 0)
		{
			weight = FW_HEAVY;
		}

		BYTE slant = FALSE;
		if ((fontInfo.styleFlags & GTLib::FontStyle_Italic) != 0)
		{
			slant = TRUE;
		}
		else if ((fontInfo.styleFlags & GTLib::FontStyle_Oblique) != 0)
		{
			slant = TRUE;
		}


		LOGFONTA logfont;
		memset(&logfont, 0, sizeof(logfont));

        // TODO: Add support for rotated fonts.
        //logfont.lfEscapement  = <rotation in tenths of degrees>;
        //logfont.lfOrientation = <rotation in tenths of degrees>;

		if (fontInfo.sizeInPoints > 0.0f)
		{
			logfont.lfHeight = -MulDiv(static_cast<int>(fontInfo.sizeInPoints), GetDeviceCaps(m_hDC, LOGPIXELSY), 72);
		}
		else
		{
			logfont.lfHeight = -static_cast<int>(fontInfo.sizeInPixels);
		}

		logfont.lfWeight  = weight;
		logfont.lfItalic  = slant;
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfQuality = (GTLib::Abs(logfont.lfHeight <= 36) ? CLEARTYPE_QUALITY : ANTIALIASED_QUALITY);      // TODO: Use CLEARTYPE_QUALITY for small fonts and ANTIALIASED_QUALITY for larger fonts.
		memcpy(logfont.lfFaceName, fontInfo.family.c_str(), (fontInfo.family.GetLength() < 31) ? fontInfo.family.GetLength() : 31);

		HFONT hFontWin32 = CreateFontIndirectA(&logfont);
		if (hFontWin32 != nullptr)
		{
            HGUIFont hFont = m_fontHandles.CreateHandle();
            if (hFont != 0)
            {
                HGDIOBJ hPrevFontWin32 = SelectObject(m_hDC, hFontWin32);

                TEXTMETRICA metrics;
                GetTextMetricsA(m_hDC, &metrics);

                m_fontHandles.AssociateObjectWithHandle(hFont, new FontGDI(hFontWin32, metrics));

                SelectObject(m_hDC, hPrevFontWin32);
                return hFont;
            }
		}

		return 0;
    }

    void GUIFontManager_GDI::DeleteFont(HGUIFont hFont)
    {
        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            // Delete the Win32 font.
            DeleteObject(pFont->hFontWin32);

            // Delete the font object.
            delete pFont;
        }
    }


    bool GUIFontManager_GDI::MeasureString(HGUIFont hFont, const char* text, size_t textLengthInChars, int &widthOut, int &heightOut)
    {
        HFONT hFontWin32 = this->GetWin32FontHandle(hFont);
        if (hFontWin32 != 0)
        {
            HGDIOBJ hPrevFontWin32 = SelectObject(m_hDC, hFontWin32);

            SIZE sizeWin32;
            if (GetTextExtentPoint32A(m_hDC, text, static_cast<int>(textLengthInChars), &sizeWin32) != 0)
            {
                widthOut  = sizeWin32.cx;
                heightOut = sizeWin32.cy;

                SelectObject(m_hDC, hPrevFontWin32);
                return true;
            }
            else
            {
                SelectObject(m_hDC, hPrevFontWin32);
                return false;
            }
        }

        return false;
    }

    int GUIFontManager_GDI::GetAscent(HGUIFont hFont) const
    {
        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            return pFont->metrics.tmAscent;
        }

        return 0;
    }

    int GUIFontManager_GDI::GetDescent(HGUIFont hFont) const
    {
        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            return pFont->metrics.tmDescent;
        }

        return 0;
    }

    int GUIFontManager_GDI::GetLineHeight(HGUIFont hFont) const
    {
        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            return pFont->metrics.tmHeight;
        }

        return 0;
    }

    bool GUIFontManager_GDI::GetGlyphMetrics(HGUIFont hFont, uint32_t character, GUIGlyphMetrics &metricsOut) const
    {
        // TODO: This is slow due to the font selection. Should use caching for this.

        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            HGDIOBJ hPrevFontWin32 = SelectObject(m_hDC, pFont->hFontWin32);

            const MAT2 transform = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};        // <-- Identity matrix

            GLYPHMETRICS metrics;
            DWORD bitmapBufferSize = GetGlyphOutlineW(m_hDC, character, GGO_NATIVE, &metrics, 0, nullptr, &transform);
            if (bitmapBufferSize != GDI_ERROR)
            {
				metricsOut.width    = metrics.gmBlackBoxX;
				metricsOut.height   = metrics.gmBlackBoxY;
				metricsOut.advance  = metrics.gmCellIncX;
				metricsOut.bearingX = metrics.gmptGlyphOrigin.x;
				metricsOut.bearingY = metrics.gmptGlyphOrigin.y;

                SelectObject(m_hDC, hPrevFontWin32);
                return true;
            }
            else
            {
                SelectObject(m_hDC, hPrevFontWin32);
                return false;
            }
        }

        return false;
    }


    HFONT GUIFontManager_GDI::GetWin32FontHandle(HGUIFont hFont)
    {
        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            return pFont->hFontWin32;
        }

        return 0;
    }
}

#endif
