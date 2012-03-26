
#include <GTEngine/FontManager.hpp>
#include <GTType/Font.hpp>
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Used to map a font with a texture.
    GTCore::Map<const GTType::Font *, Texture2D *> FontTextureMap;

    bool FontManager::Initialise()
    {
        return true;
    }

    void FontManager::Uninitialise()
    {
    }

    Texture2D * FontManager::GetTexture(const GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        assert(item != nullptr);                        // <-- with everything factored correctly, this should never be null.

        return item->value;
    }

    void FontManager::OnCreateFont(GTType::FontServer &, GTType::Font &font)
    {
        // We need a texture to associate with the font.
        Texture2D *fontTexture = new Texture2D;
   
        const GTImage::Mipmap &fontBitmap = font.GetBitmap().GetMipmap(0);

        fontTexture->Load(fontBitmap.width, fontBitmap.height, GTImage::ImageFormat_RGBA8, fontBitmap.data);
        fontTexture->SetFilter(TextureFilter_Nearest);

        // And now we map the texture to the font.
        FontTextureMap.Add(&font, fontTexture);

        // TODO: Unload the font's copy of the image data.
    }

    void FontManager::OnDeleteFont(GTType::FontServer &, GTType::Font &font)
    {
        auto item = FontTextureMap.Find(&font);
        assert(item != nullptr);                        // <-- with everything factored correctly, this should never be null.

        delete item->value;
        FontTextureMap.Remove(&font);
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