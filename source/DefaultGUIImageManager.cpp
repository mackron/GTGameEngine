// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/DefaultGUIImageManager.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Core/ImageLoader.hpp>

namespace GT
{
    DefaultGUIImageManager::DefaultGUIImageManager()
    {
    }
    
    DefaultGUIImageManager::~DefaultGUIImageManager()
    {
    }
    
    
    ///////////////////////////////////////////
    // Create
    
    GUIImageHandle DefaultGUIImageManager::CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, void* data)
    {
        return this->CreateImage(width, height, format, const_cast<const void*>(data));
    }
    
    GUIImageHandle DefaultGUIImageManager::CreateImage(unsigned int width, unsigned int height, GUIImageFormat format, const void* data)
    {
        // First, translate the format to a GTLib format. If it's not a supported format, we need to return 0.
        auto translatedFormat = ImageFormat_None;
        if (format == GUIImageFormat_RGB8)
        {
            translatedFormat = ImageFormat_RGB8;
        }
        else if (format == GUIImageFormat_RGBA8)
        {
            translatedFormat = ImageFormat_RGBA8;
        }
        else if (format == GUIImageFormat_A8)
        {
            translatedFormat = ImageFormat_R8;
        }
        else
        {
            return 0;
        }
        
        auto texture = Renderer::CreateTexture2D();
        assert(texture != nullptr);
        {
            // We don't care about local content with GUI images, but we do need to keep track of the size so we can do an accurate size
            // retrieval. To do this, we just pass null to SetData(), and then make sure the local data is deleted with DeleteLocalData().
            texture->SetData(width, height, translatedFormat, nullptr);     // <-- Important we pass null here.
            texture->DeleteLocalData();
            
            Renderer::SetTexture2DData(texture, 0, width, height, translatedFormat, data, Renderer::HasFlippedTextures());
            Renderer::SetTexture2DFilter(texture, TextureFilter_Nearest, TextureFilter_Nearest);
        }
        
        return reinterpret_cast<GUIImageHandle>(texture);
    }
    
    GUIImageHandle DefaultGUIImageManager::LoadImage(const char* filePath)
    {
        auto loader = ImageLoader::Create(filePath);
        if (loader != nullptr)
        {
            ImageFileInfo info;
            loader->GetImageInfo(info);
            
            if (info.format == ImageFormat_RGB8 || info.format == ImageFormat_RGBA8 || info.format == ImageFormat_R8)      // <-- Only supporting RGB8, RGBA8 and A8 formats.
            {
                Mipmap mipmap;
                if (loader->LoadMipmap(0, mipmap))      // <-- '0' means the base mipmap.
                {
                    GUIImageFormat format;
                    
                    if (info.format == ImageFormat_RGBA8)
                    {
                        format = GUIImageFormat_RGBA8;
                    }
                    else if (info.format == ImageFormat_R8)
                    {
                        format = GUIImageFormat_A8;
                    }
                    else
                    {
                        format = GUIImageFormat_RGB8;
                    }

                    return this->CreateImage(mipmap.width, mipmap.height, format, mipmap.data);
                }
                else
                {
                    // Error loading the mipmap. Fall through and return null.
                }
            }
            else
            {
                // Not a supported format. Fall through and return null.
            }
        }
        
        return 0;
    }
    
    
    ///////////////////////////////////////////
    // Delete

    void DefaultGUIImageManager::DeleteImage(GUIImageHandle imageToDelete)
    {
        Renderer::DeleteTexture2D(reinterpret_cast<Texture2D*>(imageToDelete));
    }


    ///////////////////////////////////////////
    // Update

    void DefaultGUIImageManager::SetImageSubData(GUIImageHandle image, unsigned int xPos, unsigned int yPos, unsigned int width, unsigned int height, void* data)
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            Renderer::SetTexture2DSubData(texture, 0, xPos, yPos, width, height, texture->GetFormat(), data, Renderer::HasFlippedTextures());
        }
    }


    ///////////////////////////////////////////
    // Properties

    void DefaultGUIImageManager::GetImageDimensions(GUIImageHandle image, unsigned int &widthOut, unsigned int &heightOut) const
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            widthOut  = texture->GetWidth();
            heightOut = texture->GetHeight();
        }
    }

    GUIImageFormat DefaultGUIImageManager::GetImageFormat(GUIImageHandle image) const
    {
        auto texture = reinterpret_cast<Texture2D*>(image);
        assert(texture != nullptr);
        {
            auto format = texture->GetFormat();
            if (format == ImageFormat_RGB8)
            {
                return GUIImageFormat_RGB8;
            }
            else if (format == ImageFormat_RGBA8)
            {
                return GUIImageFormat_RGBA8;
            }
            else if (format == ImageFormat_R8)
            {
                return GUIImageFormat_A8;
            }
        }
        
        return GUIImageFormat_Unknown;
    }
}
