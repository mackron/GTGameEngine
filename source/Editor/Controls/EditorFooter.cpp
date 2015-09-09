// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorFooter.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorFooter::EditorFooter(Editor &editor)
        : EditorControl(editor),
          m_hCommandTextBox(NULL)
    {
        GUIContext &gui = editor.GetGUI();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 32U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementVerticalAlign(hRootElement, VerticalAlign_Center);
            gui.SetElementPadding(hRootElement, 2);

            gui.SetElementBorderTop(hRootElement, 1, GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));



            // Command text box.
            m_hCommandTextBox = gui.CreateElement();
            gui.SetElementParent(m_hCommandTextBox, hRootElement);
            gui.SetElementHeightRatio(m_hCommandTextBox, 1);
            gui.SetElementWidth(m_hCommandTextBox, 480U);
            gui.SetElementBorder(m_hCommandTextBox, 1, GTLib::Colour(0.4f, 0.4f, 0.4f));
            gui.SetElementCursor(m_hCommandTextBox, GUISystemCursor::IBeam);
        }
    }

    EditorFooter::~EditorFooter()
    {
    }
}
