// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_GlyphMapManager_hpp_
#define __GT_GUI_GlyphMapManager_hpp_

#include "../GlyphMapManager.hpp"
#include "../Vector.hpp"
#include "GUIImageManager.hpp"

namespace GT
{
    /// Implementation of the glyph map manager class for working with glyph maps.
    ///
    /// This implementation simply routes everything through the given image manager, which is specified in the constructor.
    class GUIGlyphMapManager : public GTLib::GlyphMapManager
    {
    public:

        /// Constructor.
        GUIGlyphMapManager(GUIImageManager &imageManager);

        /// Destructor.
        virtual ~GUIGlyphMapManager();


        /// GlyphMapManager::CreateGlyphMap()
        GTLib::GlyphMapHandle CreateGlyphMap(unsigned int width, unsigned int height);
        
        /// GlyphMapManager::DeleteGlyphMap()
        void DeleteGlyphMap(GTLib::GlyphMapHandle glyphMap);
        
        /// GlyphMapManager::SetGlyphMapData()
        void SetGlyphMapData(GTLib::GlyphMapHandle glyphMap, int x, int y, unsigned int width, unsigned int height, void* data);

        /// GlyphMapManager::MapGlyphMapData()
        void* MapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle);

        /// GlyphMapManager::UnmapGlyphMapData()
        void UnmapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle);

        /// GlyphMapManager::GetGlyphMapDimensions()
        void GetGlyphMapDimensions(GTLib::GlyphMapHandle glyphMap, unsigned int &widthOut, unsigned int &heightOut) const;



    private:

        /// The image manager that will be used for managing the image buffers where the glyphs will be stored.
        GUIImageManager &m_imageManager;

        /// For now, glyph maps are just GTLib images.
        GTLib::Vector<ImageHandle> m_glyphMaps;



    private:    // No copying.
        GUIGlyphMapManager(const GUIGlyphMapManager &);
        GUIGlyphMapManager & operator=(const GUIGlyphMapManager &);
    };
}

#endif
