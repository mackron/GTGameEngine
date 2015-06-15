// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIGlyphMapManager.hpp>

namespace GT
{
    GUIGlyphMapManager::GUIGlyphMapManager(/*GUIImageManager &imageManager*/)
        : GlyphMapManager(),
          //m_imageManager(imageManager),
          m_glyphMaps()
    {
    }

    GUIGlyphMapManager::~GUIGlyphMapManager()
    {
    }


    GTLib::GlyphMapHandle GUIGlyphMapManager::CreateGlyphMap(unsigned int width, unsigned int height)
    {
        //GTLib::GlyphMapHandle newImage = static_cast<GTLib::GlyphMapHandle>(m_imageManager.CreateImage(width, height, GT::ImageFormat_A8, static_cast<const void*>(nullptr)));
        //this->m_glyphMaps.PushBack(newImage);

        //return newImage;

        return 0;
    }

    void GUIGlyphMapManager::DeleteGlyphMap(GTLib::GlyphMapHandle glyphMapHandle)
    {
        //m_glyphMaps.RemoveFirstOccuranceOf(glyphMapHandle);
        //m_imageManager.DeleteImage(static_cast<ImageHandle>(glyphMapHandle));
    }

    void GUIGlyphMapManager::SetGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle, int xPos, int yPos, unsigned int width, unsigned int height, void* data)
    {
        //m_imageManager.SetImageSubData(static_cast<ImageHandle>(glyphMapHandle), xPos, yPos, width, height, data);
    }

    void* GUIGlyphMapManager::MapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle)
    {
        //return m_imageManager.MapImageData(static_cast<ImageHandle>(glyphMapHandle));
        return nullptr;
    }

    void GUIGlyphMapManager::UnmapGlyphMapData(GTLib::GlyphMapHandle glyphMapHandle)
    {
        //m_imageManager.UnmapImageData(static_cast<ImageHandle>(glyphMapHandle));
    }
    
    void GUIGlyphMapManager::GetGlyphMapDimensions(GTLib::GlyphMapHandle glyphMapHandle, unsigned int &widthOut, unsigned int &heightOut) const
    {
        //m_imageManager.GetImageDimensions(static_cast<ImageHandle>(glyphMapHandle), widthOut, heightOut);
    }
}
