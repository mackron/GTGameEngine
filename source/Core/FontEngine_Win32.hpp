// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

// This file is the Win32 implementation of FontEngine.

#ifndef __GTLib_FontEngine_Win32_hpp_
#define __GTLib_FontEngine_Win32_hpp_

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
#include <GTEngine/Core/FontEngine.hpp>

#include <GTEngine/Core/windows.hpp>

namespace GTLib
{
	class FontEngine_Win32 : public FontEngine
	{
	public:

		/// Constructor.
		FontEngine_Win32(GlyphMapManager* glyphMapManager);

		/// Destructor.
		~FontEngine_Win32();


		/// FontEngine::CreateFont().
		FontHandle CreateFont(const FontInfo &fontInfo);

		/// FontEngine::DeleteFont().
		void DeleteFont(FontHandle font);


		/// FontEngine::GetFontMetrics().
		void GetFontMetrics(FontHandle font, FontMetrics &metricsOut) const;


		/// FontEngine::GetGlyph().
		GlyphHandle GetGlyph(FontHandle font, char32_t character) const;

		/// FontEngine::GetGlyphMap().
		GlyphMapHandle GetGlyphMap(GlyphHandle glyph, GTLib::Rect<float> &uvCoordsOut) const;

        /// FontEngine::GetGlyphMetrics().
        void GetGlyphMetrics(GlyphHandle glyph, GlyphMetrics &metricsOut) const;


		/// FontEngine::MeasureString().
		void MeasureString(FontHandle font, const char* inputString, MeasureStringCallback &callback) const;

        void MeasureString(FontHandle font, const char* inputString, unsigned int tabSize, MeasureString_OnHandleCharacter onHandleCharacter) const;


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
        /// @param character1 [in]  The character on the left side.
        /// @param character2 [in]  The character on the right side.
        /// @param kerningOut [out] A reference to the object that will receive the kerning vector.
        void GetKerning(FontHandle font, char32_t character1, char32_t character2, KerningVector &kerningOut) const;


	private:

		/// The device context we need to pass around to a few of the Win32 text functions.
		HDC m_hDC;


    private:    // No copying
        FontEngine_Win32(const FontEngine_Win32 &);
        FontEngine_Win32 & operator=(const FontEngine_Win32 &);
	};
}

#endif	// __WIN32__
#endif	// __GTLib_FontEngine_Win32_hpp_
