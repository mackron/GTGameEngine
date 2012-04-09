
#include <GTEngine/Texture2DLibrary.hpp>
#include <GTEngine/UserConfig.hpp>
#include <GTCore/Dictionary.hpp>

// *** Globals ***
namespace GTEngine
{
    /// The list of loaded textures, indexed by file name.
    GTCore::Dictionary<Texture2D*> LoadedTextures;
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
    }
}


namespace GTEngine
{
    Texture2D* Texture2DLibrary::Acquire(const char* fileName)
    {
        auto iTexture = LoadedTextures.Find(fileName);
        if (iTexture == nullptr)
        {
            auto newTexture = new Texture2D(fileName);
                newTexture->SetAnisotropy(UserConfig::GetInteger("Display.Textures.Anisotropy"));
                LoadedTextures.Add(fileName, newTexture);
            return newTexture;
        }
        else
        {
            assert(iTexture->value != nullptr);
            ++iTexture->value->refCount;

            return iTexture->value;
        }
    }

    Texture2D* Texture2DLibrary::Acquire(Texture2D* texture)
    {
        assert(texture != nullptr);

        ++texture->refCount;

        return texture;
    }

    void Texture2DLibrary::Unacquire(const Texture2D* texture)
    {
        assert(texture != nullptr);

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


// Engine textures.
namespace GTEngine
{
    static Texture2D* Black1x1Texture = nullptr;

    Texture2D* Texture2DLibrary::Get1x1BlackTexture()
    {
        if (Black1x1Texture == nullptr)
        {
            uint32_t texel = 0xFF000000;
            return new Texture2D(1, 1, GTImage::ImageFormat_RGBA8, &texel);
        }

        return Black1x1Texture;
    }
}
