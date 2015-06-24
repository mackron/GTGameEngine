// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorFooterControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorFooter::EditorFooter(Editor &editor)
        : EditorControl(editor)
    {
        GUIContext &gui = editor.GetGUI();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 32U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);

            gui.SetElementBorderTop(hRootElement, 1, GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));
        }
    }

    EditorFooter::~EditorFooter()
    {
    }
}
