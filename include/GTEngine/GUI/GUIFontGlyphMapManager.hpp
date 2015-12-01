// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIFontGlyphMapManager
#define GT_GUIFontGlyphMapManager

#include "GUIImageManager.hpp"             // For GUIImageHandle.
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/GlyphMapManager.hpp>

namespace GT
{
    class GUIServer;
    
    class GUIFontGlyphMapManager : public GT::GlyphMapManager
    {
    public:
    
        /// Constructor.
        GUIFontGlyphMapManager(GUIServer &server);
        
        /// Destructor.
        ~GUIFontGlyphMapManager();
    
    
        /// GlyphMapManager::CreateGlyphMap()
        GT::GlyphMapHandle CreateGlyphMap(unsigned int width, unsigned int height);
        
        /// GlyphMapManager::DeleteGlyphMap()
        void DeleteGlyphMap(GT::GlyphMapHandle glyphMapHandle);
        
        /// GlyphMapManager::SetGlyphMapData()
        void SetGlyphMapData(GT::GlyphMapHandle glyphMapHandle, int xPos, int yPos, unsigned int width, unsigned int height, void* data);

        /// GlyphMapManager::MapGlyphMapData()
        void* MapGlyphMapData(GT::GlyphMapHandle glyphMapHandle);

        /// GlyphMapManager::UnmapGlyphMapData()
        void UnmapGlyphMapData(GT::GlyphMapHandle glyphMapHandle);
        
        /// GlyphMapManager::GetGlyphMapDimensions()
        void GetGlyphMapDimensions(GT::GlyphMapHandle glyphMapHandle, unsigned int &width, unsigned int &height) const;
    
    
    private:
        
        /// A reference to the server. We need this because we are going to load the glyph maps via it's image manager.
        GUIServer &m_server;
        
        /// For now, glyph maps are just GTLib images.
        GT::Vector<GUIImageHandle> m_glyphMaps;


	private:	// No copying.
		GUIFontGlyphMapManager(const GUIFontGlyphMapManager &);
		GUIFontGlyphMapManager & operator=(const GUIFontGlyphMapManager &);
    };
}

#endif