// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuSeparator.hpp>

namespace GT
{
    EditorMenuSeparator::EditorMenuSeparator(Editor &editor, HGUIElement hParent)
        : EditorControl(editor)
    {
        m_gui.SetElementParent(m_hRootElement, hParent);
        m_gui.SetElementPadding(m_hRootElement, 4);
        m_gui.SetElementHeight(m_hRootElement, 9U);
        m_gui.SetElementWidthRatio(m_hRootElement, 1);
        m_gui.SetElementBackgroundColorBoundary(m_hRootElement, BackgroundBoundary_Inner);
        m_gui.SetElementBackgroundColor(m_hRootElement, GTLib::Colour(0.3, 0.3, 0.3));
    }

    EditorMenuSeparator::~EditorMenuSeparator()
    {
    }
}
