// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_FontGlyphMapManager_hpp_
#define __GTLib_GUI_FontGlyphMapManager_hpp_

#include "ImageManager.hpp"             // For GTGUI::ImageHandle.
#include <GTLib/Vector.hpp>
#include <GTLib/GlyphMapManager.hpp>

namespace GTGUI
{
    class Server;
    
    class FontGlyphMapManager : public GTLib::GlyphMapManager
    {
    public:
    
        /// Constructor.
        FontGlyphMapManager(Server &server);
        
        /// Destructor.
        ~FontGlyphMapManager();
    
    
        /// GlyphMapManager::CreateGlyphMap()
        GTLib::GlyphMapHandle CreateGlyphMap(unsigned int width, unsigned int height);
        
        /// GlyphMapManager::DeleteGlyphMap()
        void DeleteGlyphMap(GTLib::GlyphMapHandle glyphMapHandle);
        
        /// GlyphMapManager::SetGlyphMapData()
        void SetGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle, int xPos, int yPos, unsigned int width, unsigned int height, void* data);

        /// GlyphMapManager::MapGlyphMapData()
        void* MapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle);

        /// GlyphMapManager::UnmapGlyphMapData()
        void UnmapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle);
        
        /// GlyphMapManager::GetGlyphMapDimensions()
        void GetGlyphMapDimensions(GTLib::GlyphMapHandle glyphMapHandle, unsigned int &width, unsigned int &height) const;
    
    
    private:
        
        /// A reference to the server. We need this because we are going to load the glyph maps via it's image manager.
        Server &m_server;
        
        /// For now, glyph maps are just GTLib images.
        GTLib::Vector<ImageHandle> m_glyphMaps;


	private:	// No copying.
		FontGlyphMapManager(const FontGlyphMapManager &);
		FontGlyphMapManager & operator=(const FontGlyphMapManager &);
    };
}

#endif // __GTLib_GUI_FontGlyphMapManager_hpp_
