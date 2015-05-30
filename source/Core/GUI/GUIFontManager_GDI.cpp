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
		logfont.lfQuality = CLEARTYPE_QUALITY;
		memcpy(logfont.lfFaceName, fontInfo.family.c_str(), (fontInfo.family.GetLength() < 31) ? fontInfo.family.GetLength() : 31);

		HFONT hFontWin32 = CreateFontIndirectA(&logfont);
		if (hFontWin32 != nullptr)
		{
            HGUIFont hFont = m_fontHandles.CreateHandle();
            if (hFont != 0)
            {
                m_fontHandles.AssociateObjectWithHandle(hFont, new FontGDI(hFontWin32));
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


    bool GUIFontManager_GDI::MeasureString(HGUIFont hFont, const char* text, size_t textLengthInChars, unsigned int &widthOut, unsigned int &heightOut)
    {
        HFONT hFontWin32 = this->GetWin32FontHandle(hFont);
        if (hFontWin32 != 0)
        {
            SIZE sizeWin32;
            if (GetTextExtentPointA(m_hDC, text, static_cast<int>(textLengthInChars), &sizeWin32) != 0)
            {
                widthOut  = static_cast<unsigned int>(sizeWin32.cx);
                heightOut = static_cast<unsigned int>(sizeWin32.cy);

                return true;
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
