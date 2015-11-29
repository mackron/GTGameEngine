// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/GlyphMapManager.hpp>

namespace GT
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
