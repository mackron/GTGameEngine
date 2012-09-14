
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTCore/Dictionary.hpp>



// Engine textures.
namespace GTEngine
{
    static Texture2D* Black1x1Texture = nullptr;

    Texture2D* Texture2DLibrary::Get1x1BlackTexture()
    {
        if (Black1x1Texture == nullptr)
        {
            uint32_t texel = 0xFF000000;
            Black1x1Texture = new Texture2D(1, 1, GTImage::ImageFormat_RGBA8, &texel);
        }

        return Black1x1Texture;
    }
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
}

namespace GTEngine
{
    bool Texture2DLibrary::Startup()
    {
        return true;
    }

    void Texture2DLibrary::Shutdown()
    {
        // Textures need to be deleted.
        for (size_t i = 0; i < LoadedTextures.count; ++i)
        {
            delete LoadedTextures.buffer[i]->value;
        }
        LoadedTextures.Clear();


        delete Black1x1Texture;
        Black1x1Texture = nullptr;
    }
}


namespace GTEngine
{
    Texture2D* Texture2DLibrary::Acquire(const char* fileName)
    {
        GTCore::String absFileName;
        if (GTCore::IO::FindAbsolutePath(fileName, absFileName))
        {
            auto iTexture = LoadedTextures.Find(absFileName.c_str());
            if (iTexture == nullptr)
            {
                auto newTexture = new Texture2D(absFileName.c_str());
                if (newTexture->IsLinkedToFile())
                {
                    newTexture->SetFilter(DefaultMinFilter, DefaultMagFilter);
                    newTexture->SetAnisotropy(DefaultAnisotropy);

                    LoadedTextures.Add(absFileName.c_str(), newTexture);

                    return newTexture;
                }
                else
                {
                    GTEngine::PostError("Can not find file: %s", fileName);

                    delete newTexture;
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

    void Texture2DLibrary::Unacquire(const Texture2D* texture)
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

                delete texture;
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

                texture->LinkToFile(absFileName.c_str());
                texture->PullAllMipmaps();
            }
        }

        return false;
    }
}


namespace GTEngine
{
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

