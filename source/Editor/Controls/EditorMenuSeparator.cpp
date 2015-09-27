// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuSeparator.hpp>

namespace GT
{
    EditorMenuSeparator::EditorMenuSeparator(Editor &editor, HGUIElement hParent)
        : EditorControl(editor)
    {
        m_gui.SetElementParent(m_hRootElement, hParent);
        m_gui.SetElementPadding(m_hRootElement, 2);
        m_gui.SetElementPaddingLeft(m_hRootElement, 8);
        m_gui.SetElementPaddingRight(m_hRootElement, 8);
        m_gui.SetElementHeight(m_hRootElement, 5U);
        m_gui.SetElementWidthRatio(m_hRootElement, 1);
        m_gui.SetElementBackgroundColorBoundary(m_hRootElement, BackgroundBoundary_Inner);
        m_gui.SetElementBackgroundColor(m_hRootElement, GT::Color(0.3, 0.3, 0.3));
    }

    EditorMenuSeparator::~EditorMenuSeparator()
    {
    }
}
