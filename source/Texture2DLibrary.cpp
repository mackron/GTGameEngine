// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Texture2DLibrary.hpp>
#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/Context.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    Texture2DLibrary::Texture2DLibrary(Context &context)
        : m_context(context),
          m_loadedTextures(),
          m_defaultAnisotropy(1),
          m_defaultMinFilter(TextureFilter_LinearLinear),
          m_defaultMagFilter(TextureFilter_Linear),
          Black1x1Texture(nullptr)
    {
    }

    Texture2DLibrary::~Texture2DLibrary()
    {
    }


    bool Texture2DLibrary::Startup()
    {
        return true;
    }

    void Texture2DLibrary::Shutdown()
    {
        // Textures need to be deleted.
        for (size_t i = 0; i < m_loadedTextures.count; ++i)
        {
            Renderer::DeleteTexture2D(m_loadedTextures.buffer[i]->value);
        }
        m_loadedTextures.Clear();


        Renderer::DeleteTexture2D(Black1x1Texture);
        Black1x1Texture = nullptr;
    }


    Texture2D* Texture2DLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[EASYVFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (easypath_is_absolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                easypath_to_relative(fileName, makeRelativeTo, relativePath, sizeof(relativePath));
            }
            else
            {
                m_context.LogErrorf("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        char absFileName[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(m_context.GetVFS(), fileName, absFileName, sizeof(absFileName)))
        {
            auto iTexture = m_loadedTextures.Find(absFileName);
            if (iTexture == nullptr)
            {
                Image image(absFileName);
                if (image.IsLinkedToFile())
                {
                    image.PullAllMipmaps();     // <-- This loads the image data.


                    auto newTexture = Renderer::CreateTexture2D();
                    newTexture->SetRelativePath(relativePath);
                    newTexture->SetData(image.GetWidth(), image.GetHeight(), image.GetFormat(), image.GetBaseMipmapData());

                    Renderer::PushTexture2DData(*newTexture);
                    Renderer::GenerateTexture2DMipmaps(*newTexture);
                    Renderer::SetTexture2DFilter(*newTexture, m_defaultMinFilter, m_defaultMagFilter);
                    Renderer::SetTexture2DAnisotropy(*newTexture, m_defaultAnisotropy);
                    


                    // Local data should be cleared since it won't be needed now. The renderer will have made a copy of the data, so it's safe to delete now.
                    newTexture->DeleteLocalData();


                    m_loadedTextures.Add(absFileName, newTexture);
                    return newTexture;
                }
                else
                {
                    m_context.LogErrorf("Can not find file: %s", fileName);
                    return nullptr;
                }
            }
            else
            {
                assert(iTexture->value != nullptr);
                {
                    iTexture->value->IncrementReferenceCounter();
                }

                return iTexture->value;
            }
        }

        return nullptr;
    }

    Texture2D* Texture2DLibrary::Acquire(Texture2D* texture)
    {
        if (texture != nullptr)
        {
            texture->IncrementReferenceCounter();
        }

        return texture;
    }

    void Texture2DLibrary::Unacquire(Texture2D* texture)
    {
        if (texture != nullptr)
        {
            // If the texture is not referenced elsewhere, it needs to be deleted.
            if (texture->DecrementReferenceCounter() == 0)
            {
                // We only delete the texture if it was initially acquired by the library.
                for (size_t i = 0; i < m_loadedTextures.count; ++i)
                {
                    if (m_loadedTextures.buffer[i]->value == texture)
                    {
                        Renderer::DeleteTexture2D(texture);

                        m_loadedTextures.RemoveByIndex(i);
                        break;
                    }
                }
            }
        }
    }

    bool Texture2DLibrary::Reload(const char* fileName)
    {
        char absFileName[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(m_context.GetVFS(), fileName, absFileName, sizeof(absFileName)))
        {
            auto iTexture = m_loadedTextures.Find(absFileName);
            if (iTexture != nullptr)
            {
                auto texture = iTexture->value;
                assert(texture != nullptr);
                {
                    Image image(absFileName);
                    if (image.IsLinkedToFile())
                    {
                        texture->SetData(image.GetWidth(), image.GetHeight(), image.GetFormat(), image.GetBaseMipmapData());
                        Renderer::PushTexture2DData(*texture);
                        Renderer::GenerateTexture2DMipmaps(*texture);

                        // The local data should be deleted to save on some memory.
                        texture->DeleteLocalData();

                        return true;
                    }
                }
            }
        }

        return false;
    }



    /////////////////////////////////////////////////////
    // System/Engine textures.

    Texture2D* Texture2DLibrary::Get1x1BlackTexture()
    {
        if (Black1x1Texture == nullptr)
        {
            uint32_t texel = 0xFF000000;
            Black1x1Texture = Renderer::CreateTexture2D();

            Black1x1Texture->SetData(1, 1, ImageFormat_RGBA8, &texel);
            Renderer::PushTexture2DData(*Black1x1Texture);
        }

        return Black1x1Texture;
    }



    /////////////////////////////////////////////////////
    // Misc.

    void Texture2DLibrary::SetDefaultAnisotropy(unsigned int defaultAnisotropy)
    {
        m_defaultAnisotropy = defaultAnisotropy;
    }

    void Texture2DLibrary::SetDefaultMinFilter(TextureFilter filter)
    {
        m_defaultMinFilter = filter;
    }

    void Texture2DLibrary::SetDefaultMagFilter(TextureFilter filter)
    {
        m_defaultMagFilter = filter;
    }


    bool Texture2DLibrary::IsExtensionSupported(const char* extension)
    {
        return ImageLoader::IsExtensionSupported(extension);
    }
}

