// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/FontCache.hpp>

namespace GTGUI
{
    FontCache::FontCache(GT::FontServer &fontServer)
        : m_fontServer(fontServer), m_loadedFonts(16)
    {
    }
    
    FontCache::~FontCache()
    {
        // All loaded fonts need to be deleted.
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            m_fontServer.DeleteFont(m_loadedFonts[iLoadedFont].font);
        }
    }
    
    
    GT::Font* FontCache::AcquireFont(const GT::FontInfo &fontInfo)
    {
        // First we need to check if the font is already loaded.
        auto font = this->AcquireLoadedFont(fontInfo);
        if (font == nullptr)
        {
            // The font is not already loaded, so it needs to be loaded now.
            font = m_fontServer.CreateFont(fontInfo);
            if (font != nullptr)
            {
                m_loadedFonts.PushBack(LoadedFont(font, fontInfo));
            }
        }
        
        return font;
    }
    
    void FontCache::UnacquireFont(const GT::Font* font)
    {
        // We simply decrement the reference counter. If the new counter is at 0, we want to delete it completely.
        size_t loadedFontIndex;
        auto loadedFont = this->FindLoadedFont(font, loadedFontIndex);
        if (loadedFont != nullptr)
        {
            assert(loadedFont->referenceCount > 0);
            {
                loadedFont->referenceCount -= 1;
                if (loadedFont->referenceCount == 0)
                {
                    m_fontServer.DeleteFont(loadedFont->font);
                    m_loadedFonts.Remove(loadedFontIndex);
                }
            }
        }
    }
    
    
    
    /////////////////////////////////////////////
    // Private
    
    GT::Font* FontCache::AcquireLoadedFont(const GT::FontInfo &fontInfo)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];
            
            if (loadedFont.fontInfo.IsEqual(fontInfo))
            {
                loadedFont.referenceCount += 1;
                return loadedFont.font;
            }
        }
        
        return nullptr;
    }
    
    FontCache::LoadedFont* FontCache::FindLoadedFont(const GT::Font* font, size_t &indexOut)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];
            
            if (loadedFont.font == font)
            {
                indexOut = iLoadedFont;
                return &loadedFont;
            }
        }
        
        return nullptr;
    }
}

