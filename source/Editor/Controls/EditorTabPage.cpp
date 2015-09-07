// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorTabPage.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorTabPage::EditorTabPage(Editor &editor)
        : EditorControl(editor)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            //const EditorTheme &theme = this->GetEditor().GetTheme();

            gui.SetElementHeightRatio(hRootElement, 1.0f);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementBorder(hRootElement, 1, GTLib::Colour(0.3f, 0.3f, 0.3f));
        }
    }
}
