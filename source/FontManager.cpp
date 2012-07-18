
#include <GTEngine/FontManager.hpp>
#include <GTEngine/Rendering/Texture2D.hpp>
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
            delete FontTextureMap.buffer[i]->value;
        }
    }

    Texture2D* FontManager::GetTexture(const GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        assert(item != nullptr);                        // <-- with everything factored correctly, this should never be null.

        return item->value;
    }

    void FontManager::OnCreateFont(GTType::FontServer &, GTType::Font &font)
    {
        // We need a texture to associate with the font.
        Texture2D* fontTexture = new Texture2D;
   
        const GTImage::Mipmap &fontBitmap = font.GetBitmap().GetMipmap(0);

        fontTexture->Load(fontBitmap.width, fontBitmap.height, GTImage::ImageFormat_RGBA8, fontBitmap.data);
        fontTexture->SetFilter(TextureFilter_Nearest);

        // And now we map the texture to the font.
        FontTextureMap.Add(&font, fontTexture);
    }

    void FontManager::OnDeleteFont(GTType::FontServer &, GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        if (item != nullptr)
        {
            delete item->value;
            FontTextureMap.Remove(&font);
        }
    }
}

namespace GTEngine
{
    FontEventHandler GlobalFontEventHandler;

    FontEventHandler::FontEventHandler()
    {
    }

    FontEventHandler::~FontEventHandler()
    {
    }

    void FontEventHandler::OnCreateFont(GTType::FontServer &server, GTType::Font &font)
    {
        FontManager::OnCreateFont(server, font);
    }

    void FontEventHandler::OnDeleteFont(GTType::FontServer &server, GTType::Font &font)
    {
        FontManager::OnDeleteFont(server, font);
    }
}