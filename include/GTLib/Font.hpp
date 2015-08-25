// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GTLib_Font
#define GTLib_Font

#include "FontEngine.hpp"
#include "FontInfo.hpp"
#include "GlyphMapLayout.hpp"
#include "GlyphMapManager.hpp"
#include "GlyphMetrics.hpp"
#include "KerningVector.hpp"
#include <GTLib/Rect.hpp>
#include <GTLib/Map.hpp>
#include <cstdint>

namespace GTLib
{
    class FontServer;


    /// Structure representing a glyph.
    struct Glyph
    {
        Glyph()
            : rect(),
              uvLeft(0.0f), uvRight(0.0f), uvBottom(0.0f), uvTop(0.0f),
              metrics(), glyphMap(0), ftIndex(0)
        {
        }

        /// Helper method for setting the texture coordinates based on the current rectangle and the given glyph map dimensions.
        ///
        /// @param glyphMapWidth  [in] The width of the glyph map.
        /// @param glyphMapHeight [in] The height of the glyph map.
        ///
        /// @remarks
        ///     The 'rect' attribute must be valid for this to work correctly. This will need to be called whenever the size of
        ///     the glyph map or the glyph rectangle changes.
        void UpdateUVCoordinates(unsigned int glyphMapWidth, unsigned int glyphMapHeight)
        {
            this->uvLeft   = static_cast<float>(this->rect.left)   / glyphMapWidth;
            this->uvRight  = static_cast<float>(this->rect.right)  / glyphMapWidth;
            this->uvBottom = static_cast<float>(this->rect.bottom) / glyphMapHeight;
            this->uvTop    = static_cast<float>(this->rect.top)    / glyphMapHeight;
        }


        /// The rectangle defining the position in the texture where the visual representation of the glyph is located.
        /// The rectangle can also be used to retrieve the size of the glyph.
        GTLib::Rect<int> rect;

        // UV coordinates.
        float uvLeft;               ///< The left UV coordinate of the glyph map this glyph is located at.
        float uvRight;              ///< The right UV coordinate of the glyph map this glyph is located at.
        float uvBottom;             ///< The bottom UV coordinate of the glyph map this glyph is located at.
        float uvTop;                ///< The top UV coordinate of the glyph map this glyph is located at.

        /// The metrics of the glyph. This is needed to position characters correctly when rendering them.
        GlyphMetrics metrics;

        /// A handle to the glyph map the visual representation of this glyph is located in. Defaults to 0, which indicates the
        /// glyph has not yet been rendered.
        GlyphMapHandle glyphMap;

        /// The FreeType glyph index. This is used when the glyph needs to be referenced through FreeType.
        unsigned int ftIndex;
    };


    /**
    *   \brief  Class representing a font.
    *
    *   The font class is the main class in GTType. Fonts should be created from a font server. Do not inherit from this
    *   class. It's destructor is non-virtual to express this.
    *
    *   Finally, the size of the font is given as points. The DPI can be set, too, which is 72x72 by default.
    *
    *   Properties of a font can not be changed after it has been created. You must delete the font and then reload
    *   in order to change it's properties. Loading is a slow operation because it needs to construct the texture, etc.
    *   Therefore, don't reload fonts too frequently.
    *
    *   Fonts store the glyph visual information in a texture. The texture data is not stored internally in the interest
    *   of saving memory. Instead, the load functions will output the texture data to a seperate structure which will
    *   manage the memory itself. It will free any memory when it is destructed.
    *
    *   Do not inherit from this class. It's destructor is non-virtual to express this.
    */
    class Font
    {
    public:

        /// Destructor.
        ~Font();


        /// Retrieves a reference to the font server that owns this font.
              FontServer & GetServer()       { return this->server; }
        const FontServer & GetServer() const { return this->server; }

        /// Retrieves a reference to the internal font handle for use by the font engine.
        FontHandle GetFontHandle() const { return this->fontHandle; }


        int GetAscent() const;
        int GetDescent() const;

        /// Retrieves the height of a line for this font.
        unsigned int GetLineHeight() const;


        /// Deletes every loaded glyph and glyph map.
        void DeleteAllGlyphs();


    private:

        /// Allocates a slot for a glyph of the given size and returns a handle to the glyph map and the glyphs position in that glyph map.
        ///
        /// @param glyphWidth   [in]  The width of the glyph.
        /// @param glyphHeight  [in]  The height of the glyph.
        /// @param glyphXPosOut [out] A reference to the variable that will receive the x position of the slot with the glyph map.
        /// @param glyphYPosOut [out] A reference to the variable that will receive the y position of the slot with the glyph map.
        ///
        /// @return A handle to the glyph map the slot was inserted into.
        GlyphMapHandle AllocateGlyphMapSlot(unsigned int glyphWidth, unsigned int glyphHeight, unsigned int &glyphXPosOut, unsigned int &glyphYPosOut);


    private:

        /// A reference to the font server that owns this font. We mainly keep track of this so we can get easy access to the
        /// glyph map manager.
        FontServer &server;

        /// The font handle created by the font engine.
        FontHandle fontHandle;

        /// The metrics of the font.
        FontMetrics metrics;

        /// The list of textures making up the font. Glyphs are usually part of a single texture, but sometimes texture limits
        /// can cause the glyphs to not fit on the largest possible texture. Thus, we need to split the glyphs across multiple
        /// textures. This is usually only an issue with large font sizes where a single glyph can take up quite a bit of space.
        ///
        /// This is modified when a glyph is retrieved, but we want it to use the notion of constness from the public perspective.
        ///
        /// Every glyph map is mapped to a glyph map layout. We use this layout in determining where to insert additional glyphs
        /// into the map.
        mutable GTLib::Map<GlyphMapHandle, GlyphMapLayout> glyphMaps;

        /// The map containing pointers to the glyphs for each character.
        ///
        /// This needs to be mutable because this will be modified by GetGlyph(), which we want to be constant from the public
        /// perspective.
        mutable GTLib::Map<char32_t, Glyph*> glyphs;


    private:    // No copying.
        Font(const Font &);
        Font & operator=(const Font &);



    public:

        /// Constructor. This should not be used publically; only internally by the server.
        Font(FontServer &serverIn, FontHandle fontHandleIn);

        /// Retrieves a reference to the internal font handle.
        ///
        /// @remarks
        ///     This should only be called internally.
        FontHandle GetFontHandle() { return this->fontHandle; }
    };
}

#endif
