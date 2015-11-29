// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/FontGlyphMapManager.hpp>
#include <GTEngine/GUI/Server.hpp>

namespace GTGUI
{
    FontGlyphMapManager::FontGlyphMapManager(Server &server)
        : m_server(server), m_glyphMaps()
    {
    }

    FontGlyphMapManager::~FontGlyphMapManager()
    {
    }


    GT::GlyphMapHandle FontGlyphMapManager::CreateGlyphMap(unsigned int width, unsigned int height)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            // TODO: When we add support for an initial data pointer of null, remove this "initialData" variable.
            
            // The initial image data.
            size_t dataSize = width * height;
            auto initialData = malloc(dataSize);
            
            GT::GlyphMapHandle newImage = static_cast<GT::GlyphMapHandle>(imageManager->CreateImage(width, height, GTGUI::ImageFormat_A8, initialData));
            this->m_glyphMaps.PushBack(newImage);
            
            
            free(initialData);
            return newImage;
        }
        
        return 0;
    }

    void FontGlyphMapManager::DeleteGlyphMap(GT::GlyphMapHandle glyphMapHandle)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            m_glyphMaps.RemoveFirstOccuranceOf(glyphMapHandle);
            imageManager->DeleteImage(static_cast<ImageHandle>(glyphMapHandle));
        }
    }

    void FontGlyphMapManager::SetGlyphMapData(GT::GlyphMapHandle glyphMapHandle, int xPos, int yPos, unsigned int width, unsigned int height, void* data)
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            imageManager->SetImageSubData(static_cast<ImageHandle>(glyphMapHandle), xPos, yPos, width, height, data);
        }
    }

    void* FontGlyphMapManager::MapGlyphMapData(GT::GlyphMapHandle)
    {
        return nullptr;
    }
    void FontGlyphMapManager::UnmapGlyphMapData(GT::GlyphMapHandle)
    {
    }
    
    void FontGlyphMapManager::GetGlyphMapDimensions(GT::GlyphMapHandle glyphMapHandle, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto imageManager = m_server.GetImageManager();
        if (imageManager != nullptr)
        {
            imageManager->GetImageDimensions(static_cast<ImageHandle>(glyphMapHandle), widthOut, heightOut);
        }
    }
}
