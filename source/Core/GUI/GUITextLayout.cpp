// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Core/GUI/GUITextLayout.hpp>

namespace GT
{
    GUITextLayout::GUITextLayout(GUIFontManager &fontManager)
        : m_fontManager(fontManager)
    {
    }

    GUITextLayout::~GUITextLayout()
    {
    }


    GUIFontManager & GUITextLayout::GetFontManager()
    {
        return m_fontManager;
    }

    const GUIFontManager & GUITextLayout::GetFontManager() const
    {
        return m_fontManager;
    }
}
