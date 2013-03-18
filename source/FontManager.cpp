// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/FontManager.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTType/Font.hpp>
#include <GTCore/Map.hpp>

// TODO: This needs synchronization! Use the garbage collector.

namespace GTEngine
{
    /// Used to map a font with a texture.
    GTCore::Map<const GTType::Font*, Texture2D*> FontTextureMap;

    bool FontManager::Startup()
    {
        return true;
    }

    void FontManager::Shutdown()
    {
        for (size_t i = 0; i < FontTextureMap.count; ++i)
        {
            Renderer::DeleteTexture2D(FontTextureMap.buffer[i]->value);
        }
    }

    Texture2D* FontManager::GetTexture(const GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        assert(item != nullptr);                        // <-- with everything factored correctly, this should never be null.

        return item->value;
    }

    void FontManager::OnCreateFont(GTType::Font &font)
    {
        // We need a texture to associate with the font.
        auto fontTexture = Renderer::CreateTexture2D();
   
        const GTImage::Mipmap &fontBitmap = font.GetBitmap().GetMipmap(0);

        fontTexture->SetData(fontBitmap.width, fontBitmap.height, GTImage::ImageFormat_RGBA8, fontBitmap.data);
        Renderer::PushTexture2DData(*fontTexture);
        Renderer::SetTexture2DFilter(*fontTexture, TextureFilter_Nearest, TextureFilter_Nearest);


        // And now we map the texture to the font.
        FontTextureMap.Add(&font, fontTexture);
    }

    void FontManager::OnDeleteFont(GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        if (item != nullptr)
        {
            Renderer::DeleteTexture2D(item->value);
            FontTextureMap.Remove(&font);
        }
    }
}
