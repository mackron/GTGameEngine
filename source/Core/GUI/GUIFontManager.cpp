// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUIFontManager.hpp>
#include <GTLib/Math.hpp>

namespace GT
{
    GUIFontManager::GUIFontManager(/*GUIImageManager &imageManager*/)
        : m_glyphMapManager(/*imageManager*/),
          m_fontServer(m_glyphMapManager),
          m_fontFamilies()
    {
    }

    GUIFontManager::~GUIFontManager()
    {
        // All loaded fonts need to be deleted.
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            m_fontServer.DeleteFont(m_loadedFonts[iLoadedFont].font);
        }
    }


    GTLib::Font* GUIFontManager::AcquireFont(const GTLib::FontInfo &fontInfo)
    {
        // First we need to check if the font is already loaded.
        auto font = this->AcquireLoadedFont(fontInfo);
        if (font == nullptr)
        {
            // The font is not already loaded, so it needs to be loaded now.
            font = m_fontServer.CreateFont(fontInfo);
            if (font != nullptr)
            {
                m_loadedFonts.PushBack(LoadedFont(font, fontInfo));
            }
        }
        
        return font;
    }
    
    void GUIFontManager::UnacquireFont(const GTLib::Font* font)
    {
        // We simply decrement the reference counter. If the new counter is at 0, we want to delete it completely.
        size_t loadedFontIndex;
        auto loadedFont = this->FindLoadedFont(font, loadedFontIndex);
        if (loadedFont != nullptr)
        {
            assert(loadedFont->referenceCount > 0);
            {
                loadedFont->referenceCount -= 1;
                if (loadedFont->referenceCount == 0)
                {
                    m_fontServer.DeleteFont(loadedFont->font);
                    m_loadedFonts.Remove(loadedFontIndex);
                }
            }
        }
    }


    uint32_t GUIFontManager::EncodeFontFamily(const char* family)
    {
        uint32_t encodedValue = GTLib::Hash(family);
        m_fontFamilies.Add(encodedValue, family);

        return encodedValue;
    }

    const char* GUIFontManager::DecodeFontFamily(uint32_t family)
    {
        auto iDecodedValue = m_fontFamilies.Find(family);
        if (iDecodedValue != nullptr)
        {
            return iDecodedValue->value.c_str();
        }

        return nullptr;
    }
    
    
    
    /////////////////////////////////////////////
    // Private
    
    GTLib::Font* GUIFontManager::AcquireLoadedFont(const GTLib::FontInfo &fontInfo)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];
            
            if (loadedFont.fontInfo.IsEqual(fontInfo))
            {
                loadedFont.referenceCount += 1;
                return loadedFont.font;
            }
        }
        
        return nullptr;
    }
    
    GUIFontManager::LoadedFont* GUIFontManager::FindLoadedFont(const GTLib::Font* font, size_t &indexOut)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];
            
            if (loadedFont.font == font)
            {
                indexOut = iLoadedFont;
                return &loadedFont;
            }
        }
        
        return nullptr;
    }
}