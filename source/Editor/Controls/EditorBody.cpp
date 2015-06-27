// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorBody.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorBody::EditorBody(Editor &editor)
        : EditorControl(editor)
    {
        GUIContext &gui = editor.GetGUI();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeightRatio(hRootElement, 1.0f);
            gui.SetElementWidthRatio(hRootElement, 1.0f);


            EditorTabGroup* pDefaultTabGroup = new EditorTabGroup(editor);
            m_tabGroups.PushBack(pDefaultTabGroup);

            gui.SetElementParent(pDefaultTabGroup->GetRootGUIElement(), hRootElement);
            gui.SetElementMarginTop(pDefaultTabGroup->GetRootGUIElement(), 8);
        }
    }

    EditorBody::~EditorBody()
    {
    }
}
