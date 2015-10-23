// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#include "FontEngine_Win32.hpp"
#include <GTLib/GlyphCache.hpp>
#include <GTLib/Map.hpp>

namespace GTLib
{
	/// Structure representing a glyph in the Win32 implementation.
	struct Glyph_Win32
	{
		Glyph_Win32(const GlyphMetrics &metricsIn)
			: metrics(metricsIn), glyphMap(0),
			  uvLeft(0), uvRight(0), uvBottom(0), uvTop(0)
		{
		}


		/// The metrics of the glyph.
        GlyphMetrics metrics;

        /// A handle to the glyph map the glyph is stored in.
        GlyphMapHandle glyphMap;


        float uvLeft;               ///< The left UV coordinate of the glyph map this glyph is located at.
        float uvRight;              ///< The right UV coordinate of the glyph map this glyph is located at.
        float uvBottom;             ///< The bottom UV coordinate of the glyph map this glyph is located at.
        float uvTop;                ///< The top UV coordinate of the glyph map this glyph is located at.
	};


	/// Structure representing the value of a font handle in the Win32 implementation.
	struct Font_Win32 : public GlyphCache
	{
		Font_Win32(HFONT hFont, GlyphMapManager* glyphMapManager)
			: GlyphCache(glyphMapManager), m_hFont(hFont), m_kerningPairs()
		{
		}

		/// GlyphCache::OnGlyphCached().
        void OnGlyphCached(GlyphHandle glyph, GlyphMapHandle glyphMap, GTLib::Rect<unsigned int> &bitmapRect)
        {
			auto glyphWin32 = reinterpret_cast<Glyph_Win32*>(glyph);
            assert(glyphWin32 != nullptr);
            {
                glyphWin32->glyphMap = glyphMap;

                auto glyphMapManager = this->GetGlyphMapManager();
                if (glyphMapManager != nullptr)
                {
                    unsigned int glyphMapWidth;
                    unsigned int glyphMapHeight;
                    glyphMapManager->GetGlyphMapDimensions(glyphMap, glyphMapWidth, glyphMapHeight);

                    glyphWin32->uvLeft   = static_cast<float>(bitmapRect.left)   / glyphMapWidth;
                    glyphWin32->uvTop    = static_cast<float>(bitmapRect.top)    / glyphMapHeight;
                    glyphWin32->uvRight  = static_cast<float>(bitmapRect.right)  / glyphMapWidth;
                    glyphWin32->uvBottom = static_cast<float>(bitmapRect.bottom) / glyphMapHeight;
                }
            }
        }

		/// A handle the Win32 font.
		HFONT m_hFont;

		/// Kerning pairs. The high-order uint32_t is the character code of the first character. The low order uint32_t is the character code
		/// of the second character. The value is the kerning amount.
		GTLib::Map<uint64_t, int> m_kerningPairs;


    private:    // No copying.
        Font_Win32(const Font_Win32 &);
        Font_Win32 & operator=(const Font_Win32 &);
	};


	FontEngine_Win32::FontEngine_Win32(GlyphMapManager* glyphMapManager)
		: FontEngine(glyphMapManager), m_hDC(nullptr)
	{
		m_hDC = CreateCompatibleDC(GetDC(GetDesktopWindow()));
		SetMapMode(m_hDC, MM_TEXT);
	}

	FontEngine_Win32::~FontEngine_Win32()
	{
		DeleteDC(m_hDC);
	}


	FontHandle FontEngine_Win32::CreateFont(const FontInfo &fontInfo)
	{
		LONG weight = FW_REGULAR;
		if ((fontInfo.styleFlags & FontStyle_Bold) != 0)
		{
			weight = FW_BOLD;
		}
		else if ((fontInfo.styleFlags & FontStyle_Thin) != 0)
		{
			weight = FW_THIN;
		}
		else if ((fontInfo.styleFlags & FontStyle_ExtraLight) != 0)
		{
			weight = FW_EXTRALIGHT;
		}
		else if ((fontInfo.styleFlags & FontStyle_Light) != 0)
		{
			weight = FW_LIGHT;
		}
		else if ((fontInfo.styleFlags & FontStyle_Medium) != 0)
		{
			weight = FW_MEDIUM;
		}
		else if ((fontInfo.styleFlags & FontStyle_SemiBold) != 0)
		{
			weight = FW_SEMIBOLD;
		}
		else if ((fontInfo.styleFlags & FontStyle_ExtraBold) != 0)
		{
			weight = FW_EXTRABOLD;
		}
		else if ((fontInfo.styleFlags & FontStyle_Heavy) != 0)
		{
			weight = FW_HEAVY;
		}

		BYTE slant = FALSE;
		if ((fontInfo.styleFlags & FontStyle_Italic) != 0)
		{
			slant = TRUE;
		}
		else if ((fontInfo.styleFlags & FontStyle_Oblique) != 0)
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

		HFONT hFont = CreateFontIndirectA(&logfont);
		if (hFont != nullptr)
		{
			auto newFont = new Font_Win32(hFont, this->GetGlyphMapManager());

			// With the font created, we now need to select it and grab the kerning pairs.
			SelectObject(m_hDC, hFont);
			DWORD kerningPairsCount = GetKerningPairsA(m_hDC, INT_MAX, nullptr);
			if (kerningPairsCount > 0)
			{
				KERNINGPAIR* kerningPairs = reinterpret_cast<KERNINGPAIR*>(malloc(sizeof(KERNINGPAIR) * kerningPairsCount));
				if (kerningPairs != nullptr)
				{
					DWORD kerningPairsReturned = GetKerningPairsA(m_hDC, kerningPairsCount, kerningPairs);
					assert(kerningPairsReturned == kerningPairsCount);
					{
						for (DWORD iKerningPair = 0; iKerningPair < kerningPairsCount; ++iKerningPair)
						{
							auto &kerningPair = kerningPairs[iKerningPair];

							uint64_t kerningPairKey   = (static_cast<uint64_t>(kerningPair.wFirst) << 32) | static_cast<uint64_t>(kerningPair.wSecond);
							int      kerningPairValue = kerningPair.iKernAmount;

							newFont->m_kerningPairs.Add(kerningPairKey, kerningPairValue);
						}
					}

                    free(kerningPairs);
				}
			}


			return reinterpret_cast<FontHandle>(newFont);
		}

		return 0;
	}

	void FontEngine_Win32::DeleteFont(FontHandle font)
	{
		auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
		if (fontWin32 != nullptr)
		{
			// HFONT.
			DeleteObject(fontWin32->m_hFont);

			// Glyph cache.
			for (size_t iCachedGlyph = 0; iCachedGlyph < fontWin32->GetGlyphCount(); ++iCachedGlyph)
            {
                delete reinterpret_cast<Glyph_Win32*>(fontWin32->GetGlyphByIndex(iCachedGlyph));
            }


			// The font itself.
			delete fontWin32;
		}
	}


	void FontEngine_Win32::GetFontMetrics(FontHandle font, FontMetrics &metricsOut) const
	{
		auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
		assert(fontWin32 != nullptr);
		{
			SelectObject(m_hDC, fontWin32->m_hFont);

			TEXTMETRICA metrics;
			GetTextMetricsA(m_hDC, &metrics);

			metricsOut.ascent     = static_cast<int>(metrics.tmAscent);
			metricsOut.descent    = static_cast<int>(metrics.tmDescent);
			metricsOut.lineHeight = static_cast<int>(metrics.tmHeight);
		}
	}


	GlyphHandle FontEngine_Win32::GetGlyph(FontHandle font, char32_t character) const
	{
		auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
            auto glyphWin32 = this->FindGlyph(font, character);
            if (glyphWin32 == 0)
            {
                glyphWin32 = this->CreateAndCacheGlyph(font, character);
            }

            return glyphWin32;
        }
	}

	GlyphMapHandle FontEngine_Win32::GetGlyphMap(GlyphHandle glyph, GTLib::Rect<float> &uvCoordsOut) const
	{
		auto glyphWin32 = reinterpret_cast<Glyph_Win32*>(glyph);
		assert(glyphWin32 != nullptr);
		{
			uvCoordsOut.left   = glyphWin32->uvLeft;
            uvCoordsOut.right  = glyphWin32->uvRight;
            uvCoordsOut.bottom = glyphWin32->uvBottom;
            uvCoordsOut.top    = glyphWin32->uvTop;

			return glyphWin32->glyphMap;
		}
	}

    void FontEngine_Win32::GetGlyphMetrics(GlyphHandle glyph, GlyphMetrics &metricsOut) const
    {
        auto glyphWin32 = reinterpret_cast<Glyph_Win32*>(glyph);
        assert(glyphWin32 != nullptr);
        {
            metricsOut = glyphWin32->metrics;
        }
    }


	void FontEngine_Win32::MeasureString(FontHandle font, const char* inputString, MeasureStringCallback &callback) const
	{
		auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
            FontMetrics fontMetrics;
            this->GetFontMetrics(font, fontMetrics);


            int penPositionX    = callback.GetXStartPosition();
            int penPositionY    = callback.GetYStartPosition();
            int tabSizeInPixels = callback.GetTabSize();


            char32_t prevCharacter = 0;

            GTLib::Strings::Iterator<char> i(inputString);
            while (i)
            {
                Glyph_Win32*     glyphWin32 = nullptr;
                GlyphMetrics     glyphMetrics;
                GTLib::Rect<int> glyphRect;

                if (i.character == '\t')
                {
                    glyphMetrics.width    = 0;
                    glyphMetrics.height   = 0;
                    glyphMetrics.advance  = (((penPositionX / tabSizeInPixels) + 1) * tabSizeInPixels) - penPositionX;
                    glyphMetrics.bearingX = 0;
                    glyphMetrics.bearingY = 0;

                    glyphRect.left   = penPositionX;
                    glyphRect.top    = penPositionY;
                    glyphRect.right  = glyphRect.left + glyphMetrics.advance;
                    glyphRect.bottom = glyphRect.top;

                    prevCharacter = 0;
                }
                else
                {
                    glyphWin32 = reinterpret_cast<Glyph_Win32*>(this->GetGlyph(font, i.character));
                    if (glyphWin32 != nullptr)
                    {
                        glyphMetrics = glyphWin32->metrics;

                        // Kerning.
                        if (prevCharacter != 0)
                        {
                            KerningVector kerning;
                            this->GetKerning(font, prevCharacter, i.character, kerning);

                            penPositionX += kerning.x;
                            penPositionY += kerning.y;
                        }

                        glyphRect.left   = penPositionX + glyphMetrics.bearingX;
                        glyphRect.top    = penPositionY - glyphMetrics.bearingY + fontMetrics.ascent;
                        glyphRect.right  = glyphRect.left + glyphMetrics.width;
                        glyphRect.bottom = glyphRect.top  + glyphMetrics.height;

                        prevCharacter = i.character;
                    }
                }


                if (!callback.HandleCharacter(*this, i.character, reinterpret_cast<GlyphHandle>(glyphWin32), glyphRect, glyphMetrics, penPositionX, penPositionY))
                {
                    break;
                }

                penPositionX += glyphMetrics.advance;

                ++i;
            }
        }
	}

    void FontEngine_Win32::MeasureString(FontHandle font, const char* inputString, unsigned int tabSizeInPixels, MeasureString_OnHandleCharacter onHandleCharacter) const
    {
        auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
            FontMetrics fontMetrics;
            this->GetFontMetrics(font, fontMetrics);


            int penPositionX    = 0;
            int penPositionY    = 0;


            char32_t prevCharacter = 0;

            GTLib::Strings::Iterator<char> i(inputString);
            while (i)
            {
                Glyph_Win32*     glyphWin32 = nullptr;
                GlyphMetrics     glyphMetrics;
                GTLib::Rect<int> glyphRect;

                if (i.character == '\t')
                {
                    glyphMetrics.width    = 0;
                    glyphMetrics.height   = 0;
                    glyphMetrics.advance  = (((penPositionX / tabSizeInPixels) + 1) * tabSizeInPixels) - penPositionX;
                    glyphMetrics.bearingX = 0;
                    glyphMetrics.bearingY = 0;

                    glyphRect.left   = penPositionX;
                    glyphRect.top    = penPositionY;
                    glyphRect.right  = glyphRect.left + glyphMetrics.advance;
                    glyphRect.bottom = glyphRect.top;

                    prevCharacter = 0;
                }
                else
                {
                    glyphWin32 = reinterpret_cast<Glyph_Win32*>(this->GetGlyph(font, i.character));
                    if (glyphWin32 != nullptr)
                    {
                        glyphMetrics = glyphWin32->metrics;

                        // Kerning.
                        if (prevCharacter != 0)
                        {
                            KerningVector kerning;
                            this->GetKerning(font, prevCharacter, i.character, kerning);

                            penPositionX += kerning.x;
                            penPositionY += kerning.y;
                        }

                        glyphRect.left   = penPositionX + glyphMetrics.bearingX;
                        glyphRect.top    = penPositionY - glyphMetrics.bearingY + fontMetrics.ascent;
                        glyphRect.right  = glyphRect.left + glyphMetrics.width;
                        glyphRect.bottom = glyphRect.top  + glyphMetrics.height;

                        prevCharacter = i.character;
                    }
                }


                if (!onHandleCharacter(*this, i.character, reinterpret_cast<GlyphHandle>(glyphWin32), glyphRect, glyphMetrics, penPositionX, penPositionY))
                {
                    break;
                }

                penPositionX += glyphMetrics.advance;

                ++i;
            }
        }
    }



	///////////////////////////////////////////////////
    // Private

    GlyphHandle FontEngine_Win32::FindGlyph(FontHandle font, char32_t character) const
    {
        auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
            return fontWin32->FindGlyph(character);
        }
    }

    GlyphHandle FontEngine_Win32::CreateAndCacheGlyph(FontHandle font, char32_t character) const
    {
		GlyphHandle resultGlyph = 0;

        auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
            assert(this->FindGlyph(font, character) == 0);        // <-- The glyph should not already exist.
            {
				SelectObject(m_hDC, fontWin32->m_hFont);

				MAT2 transform = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};		// <-- Identity matrix

				GLYPHMETRICS metrics;
                DWORD bitmapBufferSize = GetGlyphOutlineA(m_hDC, character, GGO_GRAY8_BITMAP, &metrics, 0, nullptr, &transform);
                if (bitmapBufferSize != GDI_ERROR)
                {
					GlyphMetrics glyphMetrics;
					glyphMetrics.width    = metrics.gmBlackBoxX;
					glyphMetrics.height   = metrics.gmBlackBoxY;
					glyphMetrics.advance  = metrics.gmCellIncX;
					glyphMetrics.bearingX = metrics.gmptGlyphOrigin.x;
					glyphMetrics.bearingY = metrics.gmptGlyphOrigin.y;

					resultGlyph = reinterpret_cast<GlyphHandle>(new Glyph_Win32(glyphMetrics));

					if (bitmapBufferSize == 0)
					{
						fontWin32->CacheGlyph(character, resultGlyph, 0, 0, nullptr);
					}
					else
					{
						assert(bitmapBufferSize > 0);

						// The data returned by GetGlyphOutline() is DWORD aligned. We prefer a tightly packed buffer, so we need to re-arrange the data a bit. Also,
						// we want the glyph data to be bottom-up by default.
						//
						// The width and height of the bitmap is defined in the GLYPHMETRICS structure as gmBlackBoxX and gmBlackBoxX respectively.

						const unsigned int bitmapWidth  = static_cast<unsigned int>(metrics.gmBlackBoxX);
						const unsigned int bitmapHeight = static_cast<unsigned int>(metrics.gmBlackBoxY);
						const unsigned int bitmapPitch  = bitmapBufferSize / bitmapHeight;


						uint8_t* srcBitmapBuffer = reinterpret_cast<uint8_t*>(malloc(bitmapBufferSize));			// <-- DWORD-aligned.
						uint8_t* dstBitmapBuffer = reinterpret_cast<uint8_t*>(malloc(bitmapWidth * bitmapHeight));	// <-- Tightly packed.

						uint8_t* srcData = srcBitmapBuffer;
						uint8_t* dstData = dstBitmapBuffer;


						if (GetGlyphOutlineA(m_hDC, character, GGO_GRAY8_BITMAP, &metrics, bitmapBufferSize, srcBitmapBuffer, &transform) != GDI_ERROR)
						{
							// Copy the bitmap buffer.
							srcData += (bitmapHeight - 1) * bitmapPitch;	// <-- Start at the last row, move towards the top. Will end up back at the start of the buffer.

							// Unfortunately in Win32, each texel in the bitmap is in the range of 0..64. We want it in the range of 0..255, so we'll need to do a pixel-by-pixel copy. Each pixel
							// will have it's value multiplied by 4, and then subtracted by 1, making sure we don't go below 0.
							for (UINT iRow = 0; iRow < bitmapHeight; ++iRow)
							{
								for (UINT iCol = 0; iCol < bitmapWidth; ++iCol)
								{
									uint8_t &dstByte = dstData[iCol];
									uint8_t &srcByte = srcData[iCol];

									if (srcByte > 0)
									{
										dstByte = (srcByte * 4) - 1;
									}
									else
									{
										dstByte = srcByte;
									}
								}


								// Move to the next row.
								dstData += bitmapWidth;
                                //srcData += bitmapPitch;
								srcData -= bitmapPitch;       // We want a bottom-up (upside down) bitmap, so we actually want to increment by the negated pitch.
							}

							fontWin32->CacheGlyph(character, resultGlyph, bitmapWidth, bitmapHeight, dstBitmapBuffer);
						}


						free(srcBitmapBuffer);
						free(dstBitmapBuffer);
					}
				}
            }
        }

        return resultGlyph;
    }


	void FontEngine_Win32::GetKerning(FontHandle font, char32_t character1, char32_t character2, KerningVector &kerningOut) const
    {
        auto fontWin32 = reinterpret_cast<Font_Win32*>(font);
        assert(fontWin32 != nullptr);
        {
			auto iKerningPair = fontWin32->m_kerningPairs.Find((static_cast<uint64_t>(character1) << 32) | static_cast<uint64_t>(character2));
			if (iKerningPair != nullptr)
			{
				kerningOut.x = iKerningPair->value;
				kerningOut.y = 0;

			}
			else
			{
				kerningOut.x = 0;
				kerningOut.y = 0;
			}
        }
    }
}

#endif
