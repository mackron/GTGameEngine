// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Font.hpp>
#include <GTLib/FontServer.hpp>
#include <GTLib/System.hpp>
#include <GTLib/Errors.hpp>
#include <GTLib/ToString.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Math.hpp>
#include <cstdio>
#include <cstdint>

namespace GTLib
{
    Font::Font(FontServer &serverIn, FontHandle fontHandleIn)
        : server(serverIn), fontHandle(fontHandleIn), metrics(), glyphMaps(), glyphs()
    {
        server.GetFontEngine().GetFontMetrics(this->fontHandle, this->metrics);
    }
    
    Font::~Font()
    {
        // Deleting every glyph will also delete every glyph map.
        this->DeleteAllGlyphs();
    }

    int Font::GetAscent() const
    {
        return this->metrics.ascent;
    }
    
    int Font::GetDescent() const
    {
        return this->metrics.descent;
    }
    
    unsigned int Font::GetLineHeight() const
    {
        return this->metrics.lineHeight;
    }


    void Font::DeleteAllGlyphs()
    {
        // All of the glyph objects need to be deleted.
        for (size_t i = 0; i < this->glyphs.count; ++i)
        {
            delete this->glyphs.buffer[i]->value;
        }
        this->glyphs.Clear();
        
        
        // The glyph maps need to be deleted.
        auto glyphMapManager = this->server.GetGlyphMapManager();
        if (glyphMapManager != nullptr)
        {
            for (size_t iGlyphMap = 0; iGlyphMap < this->glyphMaps.count; ++iGlyphMap)
            {
                glyphMapManager->DeleteGlyphMap(this->glyphMaps.buffer[iGlyphMap]->key);
            }
            
            this->glyphMaps.Clear();
        }
    }


    //////////////////////////////////////////////
    // Private
    
    GlyphMapHandle Font::AllocateGlyphMapSlot(unsigned int glyphWidth, unsigned int glyphHeight, unsigned int &glyphXPos, unsigned int &glyphYPos)
    {
        GlyphMapHandle glyphMap = 0;
        
        auto glyphMapManager = this->server.GetGlyphMapManager();
        assert(glyphMapManager != nullptr);
        {
            for (size_t iGlyphMap = 0; iGlyphMap < this->glyphMaps.count; ++iGlyphMap)
            {
                auto &glyphMapLayout = this->glyphMaps.buffer[iGlyphMap]->value;
                
                if (glyphMapLayout.FindAndInsert(glyphWidth, glyphHeight, glyphXPos, glyphYPos))
                {
                    // We found an existing glyph map that the glyph fits in.
                    glyphMap = this->glyphMaps.buffer[iGlyphMap]->key;
                }
            }
            
            // If we haven't got a glyph map at this point we'll need to create a new one.
            if (glyphMap == 0)
            {
                unsigned int glyphMapWidth  = glyphMapManager->GetMaxWidth();
                unsigned int glyphMapHeight = glyphMapManager->GetMaxHeight();
                
                glyphMap = glyphMapManager->CreateGlyphMap(glyphMapWidth, glyphMapHeight);
                if (glyphMap != 0)
                {
                    GlyphMapLayout glyphMapLayout(glyphMapWidth, glyphMapHeight);
                    
                    if (glyphMapLayout.FindAndInsert(glyphWidth, glyphHeight, glyphXPos, glyphYPos))
                    {
                        this->glyphMaps.Add(glyphMap, glyphMapLayout);
                    }
                    else
                    {
                        // We couldn't get the glyph to fit in even a fresh glyph map. We'll just delete and return null in this case.
                        glyphMapManager->DeleteGlyphMap(glyphMap);
                        glyphMap = 0;
                    }
                }
            }
        }
        
        return glyphMap;
    }
}
