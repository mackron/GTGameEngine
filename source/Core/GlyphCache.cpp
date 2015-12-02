// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/GlyphCache.hpp>

namespace GT
{
    GlyphCache::GlyphCache(GlyphMapManager* glyphMapManager)
        : m_glyphMapManager(glyphMapManager),
          m_glyphMaps(),
          m_glyphs()
    {
    }
    
    GlyphCache::~GlyphCache()
    {
        this->Clear();
    }
    
    
    GlyphHandle GlyphCache::FindGlyph(char32_t character) const
    {
        auto iGlyph = m_glyphs.Find(character);
        if (iGlyph != nullptr)
        {
            return iGlyph->value;
        }
        
        return 0;
    }
    
    
    bool GlyphCache::CacheGlyph(char32_t character, GlyphHandle glyph, unsigned int bitmapWidth, unsigned int bitmapHeight, void* bitmapData)
    {
        assert(this->FindGlyph(character) == 0);        // <-- The glyph should not already be cached.
        {
            m_glyphs.Add(character, glyph);
            
            if (m_glyphMapManager != nullptr)
            {
                unsigned int glyphXPos;     // <-- The x position of the glyph in the glyph map.
                unsigned int glyphYPos;     // <-- The y position of the glyph in the glyph map.
                GlyphMapHandle glyphMap = this->AllocateGlyphMapSlot(bitmapWidth, bitmapHeight, glyphXPos, glyphYPos);
                if (glyphMap != 0)
                {
                    m_glyphMapManager->SetGlyphMapData(glyphMap, glyphXPos, glyphYPos, bitmapWidth, bitmapHeight, bitmapData);
                    
                    Rect<unsigned int> bitmapRect;
                    bitmapRect.left   = glyphXPos;
                    bitmapRect.top    = glyphYPos;
                    bitmapRect.right  = bitmapRect.left + bitmapWidth;
                    bitmapRect.bottom = bitmapRect.top  + bitmapHeight;
                    this->OnGlyphCached(glyph, glyphMap, bitmapRect);
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    
    size_t GlyphCache::GetGlyphCount() const
    {
        return m_glyphs.count;
    }
    
    GlyphHandle GlyphCache::GetGlyphByIndex(size_t glyphIndex) const
    {
        return m_glyphs.buffer[glyphIndex]->value;
    }
    
    
    void GlyphCache::Clear()
    {
        if (m_glyphMapManager != nullptr)
        {
            for (size_t iGlyphMap = 0; iGlyphMap < m_glyphMaps.count; ++iGlyphMap)
            {
                m_glyphMapManager->DeleteGlyphMap(m_glyphMaps.buffer[iGlyphMap]->key);
            }
        }
        
        m_glyphMaps.Clear();        
        m_glyphs.Clear();
    }
    
    
    void GlyphCache::OnGlyphCached(GlyphHandle glyph, GlyphMapHandle glyphMap, Rect<unsigned int> &bitmapRect)
    {
        (void)glyph;
        (void)glyphMap;
        (void)bitmapRect;
    }
    
    
    /////////////////////////////////////////////////
    // Private
    
    GlyphMapHandle GlyphCache::AllocateGlyphMapSlot(unsigned int glyphWidth, unsigned int glyphHeight, unsigned int &glyphXPos, unsigned int &glyphYPos)
    {
        GlyphMapHandle glyphMap = 0;
        
        if (m_glyphMapManager != nullptr)
        {
            for (size_t iGlyphMap = 0; iGlyphMap < m_glyphMaps.count; ++iGlyphMap)
            {
                auto &glyphMapLayout = m_glyphMaps.buffer[iGlyphMap]->value;
                
                if (glyphMapLayout.FindAndInsert(glyphWidth, glyphHeight, glyphXPos, glyphYPos))
                {
                    // We found an existing glyph map that the glyph fits in.
                    glyphMap = m_glyphMaps.buffer[iGlyphMap]->key;
                }
            }

            
            // If we haven't got a glyph map at this point we'll need to create a new one.
            if (glyphMap == 0)
            {
                unsigned int glyphMapWidth  = m_glyphMapManager->GetMaxWidth();
                unsigned int glyphMapHeight = m_glyphMapManager->GetMaxHeight();
                
                glyphMap = m_glyphMapManager->CreateGlyphMap(glyphMapWidth, glyphMapHeight);
                if (glyphMap != 0)
                {
                    GlyphMapLayout glyphMapLayout(glyphMapWidth, glyphMapHeight);
                    
                    if (glyphMapLayout.FindAndInsert(glyphWidth, glyphHeight, glyphXPos, glyphYPos))
                    {
                        m_glyphMaps.Add(glyphMap, glyphMapLayout);
                    }
                    else
                    {
                        // We couldn't get the glyph to fit in even a fresh glyph map. We'll just delete and return null in this case.
                        m_glyphMapManager->DeleteGlyphMap(glyphMap);
                        glyphMap = 0;
                    }
                }
            }
        }
        
        return glyphMap;
    }
}
