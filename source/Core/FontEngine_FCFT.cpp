// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

// This file is the Fontconfig/FreeType implementation of FontEngine. At the moment, this is only used by the Linux build.

#if defined(__linux__)
#include "FontEngine_FCFT.hpp"
#include <GTGE/Core/GlyphCache.hpp>
#include <GTGE/Core/Math.hpp>          // For Abs().

namespace GT
{
    struct FontHandle_FCFT : public GlyphCache
    {
        FontHandle_FCFT(FT_Face ftFaceIn, GlyphMapManager* glyphMapManager)
            : GlyphCache(glyphMapManager), ftFace(ftFaceIn)
        {
        }
        
        
        /// GlyphCache::OnGlyphCached().
        void OnGlyphCached(GlyphHandle glyph, GlyphMapHandle glyphMap, Rect<unsigned int> &bitmapRect)
        {
            auto glyphFCFT = reinterpret_cast<Glyph_FCFT*>(glyph);
            assert(glyphFCFT != nullptr);
            {
                glyphFCFT->glyphMap = glyphMap;
                
                auto glyphMapManager = this->GetGlyphMapManager();
                if (glyphMapManager != nullptr)
                {
                    unsigned int glyphMapWidth;
                    unsigned int glyphMapHeight;
                    glyphMapManager->GetGlyphMapDimensions(glyphMap, glyphMapWidth, glyphMapHeight);
                    
                    glyphFCFT->uvLeft   = static_cast<float>(bitmapRect.left)   / glyphMapWidth;
                    glyphFCFT->uvTop    = static_cast<float>(bitmapRect.top)    / glyphMapHeight;
                    glyphFCFT->uvRight  = static_cast<float>(bitmapRect.right)  / glyphMapWidth;
                    glyphFCFT->uvBottom = static_cast<float>(bitmapRect.bottom) / glyphMapHeight;
                }
            }
        }
        
        
        
        /// The FreeType handle.
        FT_Face ftFace;
        
        
    private:    // No copying.
        FontHandle_FCFT(const FontHandle_FCFT &);
        FontHandle_FCFT & operator=(const FontHandle_FCFT &);
    };
    
    
    
    FontEngine_FCFT::FontEngine_FCFT(GlyphMapManager* glyphMapManager)
        : FontEngine(glyphMapManager), m_ftLibrary(nullptr)
    {
        FT_Init_FreeType(&m_ftLibrary);
    }
    
    FontEngine_FCFT::~FontEngine_FCFT()
    {
        FT_Done_FreeType(m_ftLibrary);
    }
    
    
    FontHandle FontEngine_FCFT::CreateFont(const FontInfo &fontInfo)
    {
        FT_Face newFace = 0;
        
        if (m_ftLibrary != nullptr)
        {
            // The first step is to retrieve the file name and index of the font through Fontconfig.
            
            int slant = FC_SLANT_ROMAN;
            if ((fontInfo.styleFlags & FontStyle_Italic) != 0)
            {
                slant = FC_SLANT_ITALIC;
            }
            else if ((fontInfo.styleFlags & FontStyle_Oblique) != 0)
            {
                slant = FC_SLANT_OBLIQUE;
            }
            
            int weight = FC_WEIGHT_REGULAR;
            if ((fontInfo.styleFlags & FontStyle_Bold) != 0)
            {
                weight = FC_WEIGHT_BOLD;
            }
            else if ((fontInfo.styleFlags & FontStyle_Thin) != 0)
            {
                weight = FC_WEIGHT_THIN;
            }
            else if ((fontInfo.styleFlags & FontStyle_ExtraLight) != 0)
            {
                weight = FC_WEIGHT_EXTRALIGHT;
            }
            else if ((fontInfo.styleFlags & FontStyle_Light) != 0)
            {
                weight = FC_WEIGHT_LIGHT;
            }
            else if ((fontInfo.styleFlags & FontStyle_Medium) != 0)
            {
                weight = FC_WEIGHT_MEDIUM;
            }
            else if ((fontInfo.styleFlags & FontStyle_SemiBold) != 0)
            {
                weight = FC_WEIGHT_SEMIBOLD;
            }
            else if ((fontInfo.styleFlags & FontStyle_ExtraBold) != 0)
            {
                weight = FC_WEIGHT_EXTRABOLD;
            }
            else if ((fontInfo.styleFlags & FontStyle_Heavy) != 0)
            {
                weight = FC_WEIGHT_HEAVY;
            }
            
            
            auto pattern = FcPatternBuild
            (
                nullptr,
                FC_FAMILY, FcTypeString,  fontInfo.family.c_str(),
                FC_SLANT,  FcTypeInteger, slant,
                FC_WEIGHT, FcTypeInteger, weight,
                nullptr
            );
            FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
            FcDefaultSubstitute(pattern);
            
            FcResult devnullResult;
            auto match = FcFontMatch(nullptr, pattern, &devnullResult);
            if (match != nullptr)
            {
                FcChar8* fileName;
                int index;
                if (FcPatternGetString( match, FC_FILE,  0, &fileName) == FcResultMatch &&
                    FcPatternGetInteger(match, FC_INDEX, 0, &index)    == FcResultMatch)
                {
                    // Now we can use FreeType to actually load the font file.
                    auto error = FT_New_Face(m_ftLibrary, reinterpret_cast<const char*>(fileName), index, &newFace);
                    if (error == 0)
                    {
                        // We successfully created the face. We now need to set the size.
                        int dpiX = fontInfo.dpiX;
                        int dpiY = fontInfo.dpiY;
                        
                        if (dpiX == 0 && dpiY == 0)
                        {
                            dpiX = dpiY = 72;
                        }
                        else
                        {
                            if (dpiX == 0)
                            {
                                dpiX = dpiY;
                            }
                            else if (dpiY == 0)
                            {
                                dpiY = dpiX;
                            }
                        }
                        
                        if (fontInfo.sizeInPoints > 0.0f)
                        {
                            error = FT_Set_Char_Size(newFace, 0, static_cast<FT_F26Dot6>(fontInfo.sizeInPoints * 64), dpiX, dpiY);
                        }
                        else
                        {
                            error = FT_Set_Pixel_Sizes(newFace, 0, static_cast<FT_UInt>(fontInfo.sizeInPixels));
                        }
                        
                        if (error != 0)
                        {
                            // There was an error setting the size.
                            FT_Done_Face(newFace);
                            newFace = 0;
                        }
                    }
                    else
                    {
                        // There was an error creating the face. We'll explicity set the return value to 0 for the sake of sanity.
                        newFace = 0;
                    }
                }
                
                FcPatternDestroy(match);
            }
            
            FcPatternDestroy(pattern);
        }
        
        return reinterpret_cast<FontHandle>(new FontHandle_FCFT(newFace, this->GetGlyphMapManager()));
    }

    void FontEngine_FCFT::DeleteFont(FontHandle font)
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        if (fontFCFT != nullptr)
        {
            if (m_ftLibrary != nullptr)
            {
                // FreeType face.
                FT_Done_Face(fontFCFT->ftFace);
                
                // Glyph cache.
                for (size_t iCachedGlyph = 0; iCachedGlyph < fontFCFT->GetGlyphCount(); ++iCachedGlyph)
                {
                    delete reinterpret_cast<Glyph_FCFT*>(fontFCFT->GetGlyphByIndex(iCachedGlyph));
                }
                
                // The font itself.
                delete fontFCFT;
            }
        }
    }
    
    
    void FontEngine_FCFT::GetFontMetrics(FontHandle font, FontMetrics &metricsOut) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            // For the ascent and descent, each one seems a little bit better when we increase the size by a pixel.
            double y = static_cast<double>(fontFCFT->ftFace->size->metrics.y_ppem) / static_cast<double>(fontFCFT->ftFace->units_per_EM);
            
            metricsOut.ascent     = static_cast<int>(static_cast<double>(fontFCFT->ftFace->ascender)  * y) + 1;
            metricsOut.descent    = static_cast<int>(static_cast<double>(fontFCFT->ftFace->descender) * y) - 1;
            metricsOut.lineHeight = static_cast<unsigned int>(Abs(metricsOut.ascent) + Abs(metricsOut.descent));
        }
    }
    
    
    GlyphHandle FontEngine_FCFT::GetGlyph(FontHandle font, char32_t character) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            auto glyphFCFT = this->FindGlyph(font, character);
            if (glyphFCFT == 0)
            {
                glyphFCFT = this->CreateAndCacheGlyph(font, character);
            }
            
            return glyphFCFT;
        }
    }
    
    GlyphMapHandle FontEngine_FCFT::GetGlyphMap(GlyphHandle glyph, Rect<float> &uvCoordsOut) const
    {
        auto glyphFCFT = reinterpret_cast<Glyph_FCFT*>(glyph);
        assert(glyphFCFT != nullptr);
        {
            uvCoordsOut.left   = glyphFCFT->uvLeft;
            uvCoordsOut.right  = glyphFCFT->uvRight;
            uvCoordsOut.bottom = glyphFCFT->uvBottom;
            uvCoordsOut.top    = glyphFCFT->uvTop;
            
            return glyphFCFT->glyphMap;
        }
    }

    void FontEngine_FCFT::GetGlyphMetrics(GlyphHandle glyph, GlyphMetrics &metricsOut) const
    {
        auto glyphFCFT = reinterpret_cast<Glyph_FCFT*>(glyph);
        assert(glyphFCFT != nullptr);
        {
            metricsOut = glyphFCFT->metrics;
        }
    }
    
    
    void FontEngine_FCFT::MeasureString(FontHandle font, const char* inputString, MeasureStringCallback &callback) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            FontMetrics fontMetrics;
            this->GetFontMetrics(font, fontMetrics);
            
            
            int penPositionX    = callback.GetXStartPosition();
            int penPositionY    = callback.GetYStartPosition();
            int tabSizeInPixels = callback.GetTabSize();
            
            
            Glyph_FCFT* prevGlyphFCFT = nullptr;
            
            Strings::Iterator<char> i(inputString);
            while (i)
            {
                Glyph_FCFT*  glyphFCFT = nullptr;
                GlyphMetrics glyphMetrics;
                Rect<int>    glyphRect;
                
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
                    
                    prevGlyphFCFT = nullptr;
                }
                else
                {
                    glyphFCFT = reinterpret_cast<Glyph_FCFT*>(this->GetGlyph(font, i.character));
                    if (glyphFCFT != nullptr)
                    {
                        glyphMetrics = glyphFCFT->metrics;
                    
                        // Kerning.
                        if (prevGlyphFCFT != 0)
                        {
                            KerningVector kerning;
                            this->GetKerning(font, reinterpret_cast<GlyphHandle>(prevGlyphFCFT), reinterpret_cast<GlyphHandle>(glyphFCFT), kerning);
                            
                            penPositionX += kerning.x;
                            penPositionY += kerning.y;
                        }
                        
                        glyphRect.left   = penPositionX + glyphMetrics.bearingX;
                        glyphRect.top    = penPositionY - glyphMetrics.bearingY + fontMetrics.ascent;
                        glyphRect.right  = glyphRect.left + glyphMetrics.width;
                        glyphRect.bottom = glyphRect.top  + glyphMetrics.height;
                        
                        prevGlyphFCFT = glyphFCFT;
                    }
                }
                
                
                if (!callback.HandleCharacter(*this, i.character, reinterpret_cast<GlyphHandle>(glyphFCFT), glyphRect, glyphMetrics, penPositionX, penPositionY))
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
    
    GlyphHandle FontEngine_FCFT::FindGlyph(FontHandle font, char32_t character) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            return fontFCFT->FindGlyph(character);
        }
    }
    
    GlyphHandle FontEngine_FCFT::CreateAndCacheGlyph(FontHandle font, char32_t character) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            assert(this->FindGlyph(font, character) == 0);        // <-- The glyph should not already exist.
            {
                unsigned int glyphCharIndex = FT_Get_Char_Index(fontFCFT->ftFace, character);
                
                auto error = FT_Load_Glyph(fontFCFT->ftFace, glyphCharIndex, FT_LOAD_DEFAULT | FT_LOAD_TARGET_LCD | FT_LOAD_FORCE_AUTOHINT);
                if (!error)
                {
                    error = FT_Render_Glyph(fontFCFT->ftFace->glyph, FT_RENDER_MODE_NORMAL);  // <-- Can also use FT_RENDER_MODE_LCD, but I need to do more research.
                    if (!error)
                    {
                        // Metrics.
                        GlyphMetrics glyphMetrics;
                        glyphMetrics.width    = fontFCFT->ftFace->glyph->metrics.width  >> 6;
                        glyphMetrics.height   = fontFCFT->ftFace->glyph->metrics.height >> 6;
                        glyphMetrics.advance  = fontFCFT->ftFace->glyph->metrics.horiAdvance  >> 6;
                        glyphMetrics.bearingX = fontFCFT->ftFace->glyph->metrics.horiBearingX >> 6;
                        glyphMetrics.bearingY = fontFCFT->ftFace->glyph->metrics.horiBearingY >> 6;
                        
                        
                        // Bitmap
                        const unsigned int bitmapWidth  = static_cast<unsigned int>(fontFCFT->ftFace->glyph->bitmap.width);
                        const unsigned int bitmapHeight = static_cast<unsigned int>(fontFCFT->ftFace->glyph->bitmap.rows);
                        void*              bitmapData   = malloc(bitmapWidth * bitmapHeight);
                        
                              uint8_t* dstData = static_cast<uint8_t*>(bitmapData);
                        const uint8_t* srcData = static_cast<uint8_t*>(fontFCFT->ftFace->glyph->bitmap.buffer);
                        
                        
                        // If we're using a standard positive pitch, we want to move to the end of the buffer. This is because we're going
                        // to be reading bottom-up so that our bitmap ends up upside down.
                        if (fontFCFT->ftFace->glyph->bitmap.pitch > 0 && bitmapHeight > 0)
                        {
                            srcData += (bitmapHeight - 1) * fontFCFT->ftFace->glyph->bitmap.pitch;
                        }
                        
                        
                        // Copy row-by-row.
                        for (unsigned int iRow = 0; iRow < bitmapHeight; ++iRow)
                        {
                            memcpy(dstData, srcData, bitmapWidth);
                            
                            // Move to the next row.
                            dstData += bitmapWidth;
                            srcData -= fontFCFT->ftFace->glyph->bitmap.pitch;       // We want a bottom-up (upside down) bitmap, so we actually want to increment by the negated pitch.
                        }
                        
                        
                        GlyphHandle newGlyph = reinterpret_cast<GlyphHandle>(new Glyph_FCFT(glyphCharIndex, glyphMetrics));
                        fontFCFT->CacheGlyph(character, newGlyph, bitmapWidth, bitmapHeight, bitmapData);
                        
                        
                        free(bitmapData);
                        return newGlyph;
                    }
                }
            }
        }
        
        return 0;
    }
    
    void FontEngine_FCFT::GetKerning(FontHandle font, GlyphHandle glyph1, GlyphHandle glyph2, KerningVector &kerningOut) const
    {
        auto fontFCFT = reinterpret_cast<FontHandle_FCFT*>(font);
        assert(fontFCFT != nullptr);
        {
            FT_UInt characterIndex1 = reinterpret_cast<Glyph_FCFT*>(glyph1)->ftIndex;
            FT_UInt characterIndex2 = reinterpret_cast<Glyph_FCFT*>(glyph2)->ftIndex;
            
            FT_Vector kerningFT;
            FT_Get_Kerning(fontFCFT->ftFace, characterIndex1, characterIndex2, FT_KERNING_DEFAULT, &kerningFT);
            
            kerningOut.x = kerningFT.x >> 6;
            kerningOut.y = kerningFT.y >> 6;
        }
    }
}
#endif
