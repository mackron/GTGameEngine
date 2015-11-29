// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

// This file is the Fontconfig/FreeType implementation of FontEngine. At the moment, this is only used by the Linux build.

#ifndef __GTLib_FontEngine_FCFT_hpp_
#define __GTLib_FontEngine_FCFT_hpp_

#if defined(__linux__)
#include <GTEngine/Core/FontEngine.hpp>

#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace GTLib
{
    /// Structure representing a cached glyph in the FCFT font engine.
    struct Glyph_FCFT
    {
        Glyph_FCFT(unsigned int ftIndexIn, const GlyphMetrics &metricsIn)
            : ftIndex(ftIndexIn), metrics(metricsIn),
              glyphMap(0),
              uvLeft(0), uvRight(0), uvBottom(0), uvTop(0)
        {
        }
        
        
        /// The FreeType character index of the glyph.
        unsigned int ftIndex;
        
        /// The metrics of the glyph.
        GlyphMetrics metrics;
        
        /// A handle to the glyph map the glyph is stored in.
        GlyphMapHandle glyphMap;
        
        
        float uvLeft;               ///< The left UV coordinate of the glyph map this glyph is located at.
        float uvRight;              ///< The right UV coordinate of the glyph map this glyph is located at.
        float uvBottom;             ///< The bottom UV coordinate of the glyph map this glyph is located at.
        float uvTop;                ///< The top UV coordinate of the glyph map this glyph is located at.
    };

    
    /// The Fontconfig/FreeType implementation of the font engine.
    class FontEngine_FCFT : public FontEngine
    {
    public:
        
        /// Constructor.
        FontEngine_FCFT(GlyphMapManager* glyphMapManager);
        
        /// Destructor.
        ~FontEngine_FCFT();
        
        
        /// FontEngine::CreateFont().
        FontHandle CreateFont(const FontInfo &fontInfo);
        
        /// FontEngine::DeleteFont().
        void DeleteFont(FontHandle font);

        
        /// FontEngine::GetFontMetrics().
        void GetFontMetrics(FontHandle font, FontMetrics &metricsOut) const;


        /// FontEngine::GetGlyph().
        GlyphHandle GetGlyph(FontHandle font, char32_t character) const;

        /// FontEngine::GetGlyphMap().
        GlyphMapHandle GetGlyphMap(GlyphHandle glyph, Rect<float> &uvCoordsOut) const;

        /// FontEngine::GetGlyphMetrics().
        void GetGlyphMetrics(GlyphHandle glyph, GlyphMetrics &metricsOut) const;


        /// FontEngine::MeasureString().
        void MeasureString(FontHandle font, const char* inputString, MeasureStringCallback &callback) const;
        

    private:
        
        /// Finds an already-created glyph for the given character.
        ///
        /// @param font      [in] The font to create the glyph from.
        /// @param character [in] The character whose corresponding glyph is being retrieved.
        ///
        /// @return A pointer to the glyph if it exists; null otherwise.
        ///
        /// @remarks
        ///     This will not create the glyph if it doesn't exist. Instead it will return null.
        GlyphHandle FindGlyph(FontHandle font, char32_t character) const;
        
        /// Creates a glyph for the given character, asserting that the glyph for the character has not already been created.
        ///
        /// @param font      [in] The font to create the glyph from.
        /// @param character [in] The character whose glyph is being created.
        ///
        /// @remarks
        ///     This will cache the glyph.
        GlyphHandle CreateAndCacheGlyph(FontHandle font, char32_t character) const;
        
        
        /// Retrieves the kerning vector for the two characters.
        ///
        /// @param font       [in]  The font.
        /// @param glyph1     [in]  The glyph of the character on the left side.
        /// @param glyph2     [in]  The glyph of the character on the right side.
        /// @param kerningOut [out] A reference to the object that will receive the kerning vector.
        void GetKerning(FontHandle font, GlyphHandle glyph1, GlyphHandle glyph2, KerningVector &kerningOut) const;
        

    private:

        /// The main FreeType library object.
        FT_Library m_ftLibrary;
        
        
    private:    // No copying.
        FontEngine_FCFT(const FontEngine_FCFT &);
        FontEngine_FCFT & operator=(const FontEngine_FCFT &);
    };
}
#endif  // __linux__
#endif  // __GTLib_FontEngine_FCFT_hpp_
