// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorHeaderControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorHeader::EditorHeader(Editor &editor)
        : EditorControl(editor),
          m_menuBar(editor)
    {
        GUIContext &gui = editor.GetGUI();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 26U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementVerticalAlign(hRootElement, VerticalAlign_Center);
            gui.SetElementBorderBottom(hRootElement, 1, GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));

            gui.SetElementParent(m_menuBar.GetRootGUIElement(), this->GetRootGUIElement());
        }
    }

    EditorHeader::~EditorHeader()
    {
    }
}
