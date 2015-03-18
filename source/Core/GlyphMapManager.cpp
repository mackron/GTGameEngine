// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/GlyphMapManager.hpp>

namespace GTLib
{
    GlyphMapManager::GlyphMapManager()
    {
    }
    
    GlyphMapManager::~GlyphMapManager()
    {
    }
    
    
    unsigned int GlyphMapManager::GetMaxWidth() const
    {
        return 256;
    }
    
    unsigned int GlyphMapManager::GetMaxHeight() const
    {
        return 256;
    }
}
