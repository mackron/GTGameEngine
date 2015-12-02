// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/FontEngine.hpp>

namespace GT
{
    FontEngine::FontEngine(GlyphMapManager* glyphMapManager)
        : m_glyphMapManager(glyphMapManager)
    {
    }
    
    FontEngine::~FontEngine()
    {
    }


    void* FontEngine::MapGlyphMapData(GlyphMapHandle hGlyphMap) const
    {
        if (m_glyphMapManager != nullptr)
        {
            return m_glyphMapManager->MapGlyphMapData(hGlyphMap);
        }

        return nullptr;
    }

    void FontEngine::UnmapGlyphMapData(GlyphMapHandle hGlyphMap) const
    {
        if (m_glyphMapManager != nullptr)
        {
            m_glyphMapManager->UnmapGlyphMapData(hGlyphMap);
        }
    }


    void FontEngine::GetGlyphMapSize(GlyphMapHandle hGlyphMap, unsigned int &widthOut, unsigned int &heightOut) const
    {
        if (m_glyphMapManager != nullptr)
        {
            m_glyphMapManager->GetGlyphMapDimensions(hGlyphMap, widthOut, heightOut);
        }
        else
        {
            widthOut  = 0;
            heightOut = 0;
        }
    }
}
