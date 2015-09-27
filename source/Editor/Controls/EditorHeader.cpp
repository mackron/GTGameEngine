// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorHeader.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorHeader::EditorHeader(Editor &editor, HWindow hParentWindow)
        : EditorControl(editor),
          m_menuBar(editor, hParentWindow)
    {
        GUIContext &gui = editor.GetGUI();
        const EditorTheme &theme = editor.GetTheme();

        HGUIElement hRootElement = this->GetRootElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 26U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementVerticalAlign(hRootElement, VerticalAlign_Center);
            gui.SetElementBackgroundColor(hRootElement, theme.backgroundMid);
            gui.SetElementBorderBottom(hRootElement, 1, GT::Color(0.2f, 0.2f, 0.2f, 1.0f));
            gui.SetElementPaddingLeft(hRootElement, 2);

            gui.SetElementParent(m_menuBar.GetRootElement(), this->GetRootElement());
        }
    }

    EditorHeader::~EditorHeader()
    {
    }
}
