// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIFontManager_GDI.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/FontStyles.hpp>

namespace GT
{
    GUIFontManager_GDI::GUIFontManager_GDI(uint32_t options)
        : GUIFontManager(options),
          m_hDC(NULL),
          m_hTextBitmap(0), m_textBitmapWidth(0), m_textBitmapHeight(0)
    {
        m_hDC = CreateCompatibleDC(GetDC(GetDesktopWindow()));
		SetMapMode(m_hDC, MM_TEXT);
    }

    GUIFontManager_GDI::~GUIFontManager_GDI()
    {
        // All loaded fonts need to be deleted.
        this->DeleteAllFonts();

        if (m_hTextBitmap != 0)
        {
            DeleteObject(m_hTextBitmap);
        }

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

        logfont.lfHeight  = -static_cast<int>(fontInfo.size);
		logfont.lfWeight  = weight;
		logfont.lfItalic  = slant;
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfQuality = (GTLib::Abs(logfont.lfHeight) > 36 || (this->GetOptions() & FontOption_NoClearType) != 0) ? ANTIALIASED_QUALITY : CLEARTYPE_QUALITY;
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

            // The string needs to be converted to a wchar_t string before we'll be able to accurately measure.
            BOOL result = FALSE;
            SIZE sizeWin32;
            sizeWin32.cx = 0;
            sizeWin32.cy = 0;

            wchar_t textW[GT_MAX_TEXT_RUN_SIZE_IN_BYTES];
            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, text, int(GTLib::Strings::SizeInTsFromCharacterCount(text, textLengthInChars)), textW, GT_MAX_TEXT_RUN_SIZE_IN_BYTES - 1);
            if (bufferSize > 0)
            {
                result = GetTextExtentPoint32W(m_hDC, textW, bufferSize, &sizeWin32);
            }


            if (result != 0)
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

    bool GUIFontManager_GDI::GetGlyphMetrics(HGUIFont hFont, char32_t character, GUIGlyphMetrics &metricsOut) const
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

    bool GUIFontManager_GDI::GetTextCursorPositionFromPoint(HGUIFont hFont, const char* text, unsigned int textLengthInChars, unsigned int maxWidth, int inputPosX, int &textCursorPosXOut, unsigned int &characterIndexOut) const
    {
        bool successful = false;

        FontGDI* pFont = m_fontHandles.GetAssociatedObject(hFont);
        if (pFont != nullptr)
        {
            HGDIOBJ hPrevFontWin32 = SelectObject(m_hDC, pFont->hFontWin32);

            GCP_RESULTS results;
            ZeroMemory(&results, sizeof(results));
            results.lStructSize = sizeof(results);
            results.nGlyphs     = textLengthInChars;
            
            wchar_t textW[GT_MAX_TEXT_RUN_SIZE_IN_BYTES];
            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, text, int(results.nGlyphs), textW, GT_MAX_TEXT_RUN_SIZE_IN_BYTES - 1);
            if (bufferSize > 0)
            {
                textW[bufferSize] = '\0';

                results.lpCaretPos = reinterpret_cast<int*>(malloc(sizeof(int) * results.nGlyphs));
                if (results.lpCaretPos != nullptr)
                {
                    if (GetCharacterPlacementW(m_hDC, textW, results.nGlyphs, int(maxWidth), &results, GCP_MAXEXTENT | GCP_USEKERNING) != 0)
                    {
                        characterIndexOut = 0;
                        textCursorPosXOut = 0;

                        for (unsigned int iChar = 0; iChar < results.nGlyphs; ++iChar)
                        {
                            int charBoundsLeft = results.lpCaretPos[iChar];
                            int charBoundsRight = 0;
                            if (iChar < results.nGlyphs - 1) {
                                charBoundsRight = results.lpCaretPos[iChar + 1];
                            } else {
                                charBoundsRight = int(maxWidth);
                            }

                            if (inputPosX >= charBoundsLeft && inputPosX <= charBoundsRight)
                            {
                                // The input position is somewhere on top of this character. If it's positioned on the left side of the character, set the output
                                // value to the character at iChar. Otherwise it should be set to the character at iChar + 1.
                                int charBoundsRightHalf = charBoundsLeft + int(ceil(((charBoundsRight - charBoundsLeft) / 2.0f)));
                                if (inputPosX <= charBoundsRightHalf) {
                                    textCursorPosXOut = charBoundsLeft;
                                    characterIndexOut = iChar;
                                } else {
                                    textCursorPosXOut = charBoundsRight;
                                    characterIndexOut = iChar + 1;
                                }
                            }
                        }

                        successful = true;
                    }

                    free(results.lpCaretPos);
                }
            }


            SelectObject(m_hDC, hPrevFontWin32);
        }

        return successful;
    }


    bool GUIFontManager_GDI::DrawTextToBuffer(HGUIFont hFont, const char* text, size_t textLengthChars, GTLib::Colour color, void* bufferOut, size_t bufferOutSize)
    {
        // The string first needs to be measured to ensure the DIB bitmap is large enough.
        int textWidth;
        int textHeight;
        if (this->MeasureString(hFont, text, textLengthChars, textWidth, textHeight))
        {
            if (bufferOutSize < static_cast<size_t>(textWidth*textHeight*4))
            {
                // Output buffer is too small.
                return false;
            }


            HFONT hFontWin32 = this->GetWin32FontHandle(hFont);

            // Now we need to check if we need to create the bitmap.
            if (m_hTextBitmap == 0 || static_cast<unsigned int>(textWidth) > m_textBitmapWidth || static_cast<unsigned int>(textHeight) > m_textBitmapHeight)
            {
                if (m_hTextBitmap == 0)
                {
                    DeleteObject(m_hTextBitmap);
                    m_hTextBitmap = 0;
                    m_pTextBitmapData = nullptr;
                }

                m_textBitmapWidth  = (static_cast<unsigned int>(textWidth)  > m_textBitmapWidth)  ? static_cast<unsigned int>(textWidth)  : m_textBitmapWidth;
                m_textBitmapHeight = (static_cast<unsigned int>(textHeight) > m_textBitmapHeight) ? static_cast<unsigned int>(textHeight) : m_textBitmapHeight;

                BITMAPINFO bmi;
                ZeroMemory(&bmi, sizeof(bmi));
                bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
                bmi.bmiHeader.biWidth       = m_textBitmapWidth;
                bmi.bmiHeader.biHeight      = m_textBitmapHeight;
                bmi.bmiHeader.biPlanes      = 1;
                bmi.bmiHeader.biBitCount    = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                m_hTextBitmap = CreateDIBSection(m_hDC, &bmi, DIB_RGB_COLORS, &m_pTextBitmapData, NULL, 0);
                if (m_hTextBitmap == 0)
                {
                    // An error occured while creating the DIB section.
                    return false;
                }
            }


            // At this point we should have a valid DIB section, so now we need to draw to it. Unfortunately GDI does not have any real notion of transparency, so
            // we need to do a little bit more work to get that working. What we do is clear the bitmap to black, and then draw the text white. We then go over
            // every pixel and use it's colour as the transparency value. To get the RGB colours, we just multiply with the requested colour.
            //
            // Note that this technique won't look right with ClearType fonts, so that will need to be disabled for this to look right.
            GdiFlush();

            // Clear.
            for (int iRow = m_textBitmapHeight - textHeight; iRow < static_cast<int>(m_textBitmapHeight); ++iRow)
            {
                for (int iCol = 0; iCol < textWidth; ++iCol)
                {
                    uint32_t* pTextBitmapData32 = reinterpret_cast<uint32_t*>(m_pTextBitmapData);
                    assert(pTextBitmapData32 != nullptr);

                    pTextBitmapData32[(iRow * m_textBitmapWidth) + iCol] = 0x00000000;
                }
            }


            // Draw the text.
            //
            // We need to make sure the DIB section is made current before drawing (and restored afterwards).
            HGDIOBJ hOldBitmap = SelectObject(m_hDC, m_hTextBitmap);
            HGDIOBJ hOldFont   = SelectObject(m_hDC, hFontWin32);

            SetBkMode(m_hDC, TRANSPARENT);
            SetTextColor(m_hDC, RGB(255, 255, 255));

            // We know that the input string will not be longer than GT_MAX_TEXT_RUN_SIZE_IN_BYTES.
            wchar_t textW[GT_MAX_TEXT_RUN_SIZE_IN_BYTES];
            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, text, int(GTLib::Strings::SizeInTsFromCharacterCount(text, textLengthChars)), textW, GT_MAX_TEXT_RUN_SIZE_IN_BYTES - 1);
            if (bufferSize > 0)
            {
                TextOutW(m_hDC, 0, 0, textW, bufferSize);
            }


            SelectObject(m_hDC, hOldFont);
            SelectObject(m_hDC, hOldBitmap);


            // Copy to output buffer.
            GdiFlush();

            uint32_t b = static_cast<uint32_t>(color.b * 255); if (b > 255) { b = 255; }
            uint32_t g = static_cast<uint32_t>(color.g * 255); if (g > 255) { g = 255; }
            uint32_t r = static_cast<uint32_t>(color.r * 255); if (r > 255) { r = 255; }
            uint32_t rgb = (b << 16) | (g << 8) | (r << 0);

            for (int iRow = 0; iRow < textHeight; ++iRow)
            {
                for (int iCol = 0; iCol < textWidth; ++iCol)
                {
                    uint32_t  srcTexel = reinterpret_cast<uint32_t*>(m_pTextBitmapData)[((iRow + (m_textBitmapHeight - textHeight)) * m_textBitmapWidth) + iCol];
                    uint32_t &dstTexel = reinterpret_cast<uint32_t*>(bufferOut        )[(iRow * textWidth)         + iCol];

                    dstTexel = ((srcTexel & 0x00FF0000) << 8) | rgb;
                }
            }


            // Everything should be good at this point...
            return true;
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
