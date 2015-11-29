// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_FontGlyphMapManager
#define GT_GUI_FontGlyphMapManager

#include "ImageManager.hpp"             // For GTGUI::ImageHandle.
#include <GTEngine/Core/Vector.hpp>
#include <GTEngine/Core/GlyphMapManager.hpp>

namespace GTGUI
{
    class Server;
    
    class FontGlyphMapManager : public GT::GlyphMapManager
    {
    public:
    
        /// Constructor.
        FontGlyphMapManager(Server &server);
        
        /// Destructor.
        ~FontGlyphMapManager();
    
    
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
        Server &m_server;
        
        /// For now, glyph maps are just GTLib images.
        GT::Vector<ImageHandle> m_glyphMaps;


	private:	// No copying.
		FontGlyphMapManager(const FontGlyphMapManager &);
		FontGlyphMapManager & operator=(const FontGlyphMapManager &);
    };
}

#endif // GT_GUI_FontGlyphMapManager
