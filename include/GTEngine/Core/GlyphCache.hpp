// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GlyphCache
#define GT_GlyphCache

#include "FontEngine.hpp"
#include "GlyphMapLayout.hpp"
#include <GTEngine/Core/Map.hpp>

namespace GT
{
    /// Platform independant class for caching glyphs.
    ///
    /// This will cache both GlyphHandle's (mapped to char32_t character codes) and the bitmap data of the glyph. When caching a glyph,
    /// you pass the glyph's bitmap data. The bitmap data is passed to the glyph map manager, which will be assigned in the constructor
    /// of this class. This class is responsible for finding an appropriate slot in a glyph map and creating a new glyph map when 
    /// required.
    ///
    /// Because this class is responsible for finding a slot for the glyph in the glyph map, when a glyph is cached, the position of
    /// the slot will be returned.
    ///
    /// A glyph cache will be assigned per-font.
    ///
    /// Because this class does not actually create each glyph, it is the responsibility of higher-level classes to delete each glyph.
    /// This class will not delete any glyphs in the destructor, but it will delete the glyph maps and unset the glyph map handles in
    /// each glyph that is still cached.
    ///
    /// There is no notion of "uncaching" glyphs in this class - the idea is that once a glyph is cached, it is always cached for the
    /// rest of time. The main reason for this is it avoids added complexity with the glyph map management. The cached can be completely
    /// cleared with Clear().
    class GlyphCache
    {
    public:
        
        /// Constructor.
        GlyphCache(GlyphMapManager* glyphMapManager);
        
        /// Destructor.
        virtual ~GlyphCache();
        
        
        /// Retrieves a pointer to the glyph map manager.
        GlyphMapManager* GetGlyphMapManager() { return m_glyphMapManager; }
        
        
        /// Finds a cached glyph handle by it's character code.
        ///
        /// @param character [in] The character code of the glyph being retrieved.
        ///
        /// @return A handle to cached glyph if it exists in the cache, 0 otherwise.
        GlyphHandle FindGlyph(char32_t character) const;
        
        /// Caches a glyph.
        ///
        /// @param character       [in]  The character code to map the glyph to.
        /// @param glyph           [in]  The glyph to cache.
        /// @param bitmapWidth     [in]  The width of the glyph's bitmap.
        /// @param bitmapHeight    [in]  The height of the glyph's bitmap.
        /// @param bitmapData      [in]  A pointer to the glyph's bitmap data.
        ///
        /// @return True if the glyph was successfully cached; false otherwise.
        ///
        /// @remarks
        ///     This method will fail if the glyph's bitmap is too bit to fit inside a glyph map.
        ///     @par
        ///     This method asserts that the glyph is not already cached.
        ///     @par
        ///     This method will call the OnChanged() method in the glyph map callback. This is how the callee can keep track of the position
        ///     in the glyph map this glyph's bitmap data is stored.
        ///     @par
        ///     The bitmap data is intentionally a non-const pointer because the the glyph map manager implementation should be able to modify
        ///     the data in place if needed. This is mainly to allow the manager to flip the image data upside-down if needed.
        bool CacheGlyph(char32_t character, GlyphHandle glyph, unsigned int bitmapWidth, unsigned int bitmapHeight, void* bitmapData);


        /// Retrieves the number of glyphs that are currently cached.
        ///
        /// @return The number of glyphs that are currently cached.
        size_t GetGlyphCount() const;
        
        /// Retrieves a glyph handle by it's index.
        ///
        /// @param glyphIndex [in] The index of the glyph to retrieve.
        GlyphHandle GetGlyphByIndex(size_t glyphIndex) const;


        /// Clears the cache.
        void Clear();
        
        
        
        ///////////////////////////////////////////////////
        // Virtual methods.
        
        /// Called when a glyph is cached.
        ///
        /// @param glyph      [in] A handle to the glyph.
        /// @param glyphMap   [in] The glyph map the glyph's bitmap data is placed in.
        /// @param bitmapRect [in] The rectangle region the glyph's bitmap is occupying in the glyph map.
        virtual void OnGlyphCached(GlyphHandle glyph, GlyphMapHandle glyphMap, Rect<unsigned int> &bitmapRect);
        
        
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
        
        /// A pointer to the glyph map manager. This is where each glyph's bitmap data will be passed through to.
        GlyphMapManager* m_glyphMapManager;

        /// The list of textures making up the font. Glyphs are usually part of a single texture, but sometimes texture limits
        /// can cause the glyphs to not fit on the largest possible texture. Thus, we need to split the glyphs across multiple
        /// textures. This is usually only an issue with large font sizes where a single glyph can take up quite a bit of space.
        ///
        /// Every glyph map is mapped to a glyph map layout. We use this layout in determining where to insert additional glyphs
        /// into the map.
        Map<GlyphMapHandle, GlyphMapLayout> m_glyphMaps;
        
        /// The list of glyph handles, mapped to their character codes.
        Map<char32_t, GlyphHandle> m_glyphs;
        
        
    private:    // No copying.
        GlyphCache(const GlyphCache &);
        GlyphCache & operator=(const GlyphCache &);
    };
}

#endif // GT_GlyphCache
