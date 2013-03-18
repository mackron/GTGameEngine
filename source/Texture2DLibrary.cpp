// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/Logging.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTCore/Dictionary.hpp>
#include <GTCore/Path.hpp>



// Engine textures.
namespace GTEngine
{
    

    
}


// *** Globals ***
namespace GTEngine
{
    /// The list of loaded textures, indexed by file name.
    static GTCore::Dictionary<Texture2D*> LoadedTextures;

    /// The default level of anistropy to apply to all textures.
    static unsigned int DefaultAnisotropy = 1;

    /// The default minification filter to apply to textures.
    static TextureFilter DefaultMinFilter = TextureFilter_LinearLinear;

    /// The default magnification filter to apply to textures.
    static TextureFilter DefaultMagFilter = TextureFilter_Linear;


    // Global Textures.
    static Texture2D* Black1x1Texture = nullptr;


    /////////////////////////////////////////////////////
    // Startup/Shutdown

    bool Texture2DLibrary::Startup()
    {
        return true;
    }

    void Texture2DLibrary::Shutdown()
    {
        // Textures need to be deleted.
        for (size_t i = 0; i < LoadedTextures.count; ++i)
        {
            Renderer2::DeleteTexture2D(LoadedTextures.buffer[i]->value);
        }
        LoadedTextures.Clear();


        Renderer2::DeleteTexture2D(Black1x1Texture);
        Black1x1Texture = nullptr;
    }


    Texture2D* Texture2DLibrary::Acquire(const char* fileName, const char* makeRelativeTo)
    {
        GTCore::String relativePath(fileName);

        if (GTCore::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                relativePath = GTCore::IO::ToRelativePath(fileName, makeRelativeTo);
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        GTCore::String absFileName;
        if (GTCore::IO::FindAbsolutePath(fileName, absFileName))
        {
            auto iTexture = LoadedTextures.Find(absFileName.c_str());
            if (iTexture == nullptr)
            {
                GTImage::Image image(absFileName.c_str());
                if (image.IsLinkedToFile())
                {
                    image.PullAllMipmaps();     // <-- This loads the image data.


                    auto newTexture = Renderer2::CreateTexture2D();
                    newTexture->SetRelativePath(relativePath.c_str());
                    newTexture->SetData(image.GetWidth(), image.GetHeight(), image.GetFormat(), image.GetBaseMipmapData());

                    Renderer2::PushTexture2DData(*newTexture);
                    Renderer2::GenerateTexture2DMipmaps(*newTexture);
                    Renderer2::SetTexture2DFilter(*newTexture, DefaultMinFilter, DefaultMagFilter);
                    Renderer2::SetTexture2DAnisotropy(*newTexture, DefaultAnisotropy);
                    


                    // Local data should be cleared since it won't be needed now. The renderer will have made a copy of the data, so it's safe to delete now.
                    newTexture->DeleteLocalData();


                    LoadedTextures.Add(absFileName.c_str(), newTexture);
                    return newTexture;
                }
                else
                {
                    GTEngine::PostError("Can not find file: %s", fileName);
                    return nullptr;
                }
            }
            else
            {
                assert(iTexture->value != nullptr);
                ++iTexture->value->refCount;

                return iTexture->value;
            }
        }

        return nullptr;
    }

    Texture2D* Texture2DLibrary::Acquire(Texture2D* texture)
    {
        assert(texture != nullptr);

        ++texture->refCount;

        return texture;
    }

    void Texture2DLibrary::Unacquire(Texture2D* texture)
    {
        if (texture != nullptr)
        {
            // If the texture is not referenced elsewhere, it needs to be deleted.
            if (texture->refCount == 1)
            {
                for (size_t i = 0; i < LoadedTextures.count; ++i)
                {
                    if (LoadedTextures.buffer[i]->value == texture)
                    {
                        LoadedTextures.RemoveByIndex(i);
                        break;
                    }
                }

                Renderer2::DeleteTexture2D(texture);
            }
            else
            {
                --texture->refCount;
            }
        }
    }

    bool Texture2DLibrary::Reload(const char* fileName)
    {
        GTCore::String absFileName;
        if (GTCore::IO::FindAbsolutePath(fileName, absFileName))
        {
            auto iTexture = LoadedTextures.Find(absFileName.c_str());
            if (iTexture != nullptr)
            {
                auto texture = iTexture->value;
                assert(texture != nullptr);
                {
                    GTImage::Image image(absFileName.c_str());
                    if (image.IsLinkedToFile())
                    {
                        texture->SetData(image.GetWidth(), image.GetHeight(), image.GetFormat(), image.GetBaseMipmapData());
                        Renderer2::PushTexture2DData(*texture);
                        Renderer2::GenerateTexture2DMipmaps(*texture);

                        // The local data should be deleted to save on some memory.
                        texture->DeleteLocalData();
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
            Black1x1Texture = Renderer2::CreateTexture2D();

            Black1x1Texture->SetData(1, 1, GTImage::ImageFormat_RGBA8, &texel);
            Renderer2::PushTexture2DData(*Black1x1Texture);
        }

        return Black1x1Texture;
    }



    /////////////////////////////////////////////////////
    // Misc.

    void Texture2DLibrary::SetDefaultAnisotropy(unsigned int defaultAnisotropy)
    {
        DefaultAnisotropy = defaultAnisotropy;
    }

    void Texture2DLibrary::SetDefaultMinFilter(TextureFilter filter)
    {
        DefaultMinFilter = filter;
    }

    void Texture2DLibrary::SetDefaultMagFilter(TextureFilter filter)
    {
        DefaultMagFilter = filter;
    }


    bool Texture2DLibrary::IsExtensionSupported(const char* extension)
    {
        return GTImage::Loader::IsExtensionSupported(extension);
    }
}

