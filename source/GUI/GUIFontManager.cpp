// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/GUIFontManager.hpp>
#include <GTLib/Math.hpp>

namespace GT
{
    GUIFontManager::GUIFontManager(uint32_t options)
        : m_options(options), m_fontFamilies()
    {
    }

    GUIFontManager::~GUIFontManager()
    {
    }


    uint32_t GUIFontManager::GetOptions() const
    {
        return m_options;
    }


    HGUIFont GUIFontManager::AcquireFont(const GUIFontInfo &fontInfo)
    {
        // First we need to check if the font is already loaded.
        auto hFont = this->AcquireLoadedFont(fontInfo);
        if (hFont == 0)
        {
            // The font is not already loaded, so it needs to be loaded now.
            hFont = this->CreateFont(fontInfo);
            if (hFont != 0)
            {
                m_loadedFonts.PushBack(LoadedFont(hFont, fontInfo));
            }
        }

        return hFont;
    }

    void GUIFontManager::UnacquireFont(const HGUIFont hFont)
    {
        // We simply decrement the reference counter. If the new counter is at 0, we want to delete it completely.
        size_t loadedFontIndex;
        auto loadedFont = this->FindLoadedFont(hFont, loadedFontIndex);
        if (loadedFont != nullptr)
        {
            assert(loadedFont->referenceCount > 0);
            {
                loadedFont->referenceCount -= 1;
                if (loadedFont->referenceCount == 0)
                {
                    this->DeleteFont(loadedFont->hFont);
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


    void GUIFontManager::DeleteAllFonts()
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            this->DeleteFont(m_loadedFonts[iLoadedFont].hFont);
        }

        m_loadedFonts.Clear();
    }


    /////////////////////////////////////////////
    // Private

    HGUIFont GUIFontManager::AcquireLoadedFont(const GUIFontInfo &fontInfo)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];

            if (loadedFont.fontInfo.IsEqual(fontInfo))
            {
                loadedFont.referenceCount += 1;
                return loadedFont.hFont;
            }
        }

        return 0;
    }

    GUIFontManager::LoadedFont* GUIFontManager::FindLoadedFont(HGUIFont hFont, size_t &indexOut)
    {
        for (size_t iLoadedFont = 0; iLoadedFont < m_loadedFonts.count; ++iLoadedFont)
        {
            auto &loadedFont = m_loadedFonts[iLoadedFont];

            if (loadedFont.hFont == hFont)
            {
                indexOut = iLoadedFont;
                return &loadedFont;
            }
        }

        return nullptr;
    }
}
