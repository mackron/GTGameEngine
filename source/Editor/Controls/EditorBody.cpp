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
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.EnableElementChildWidthFlexing(hRootElement);


            m_hLeftContainer = gui.CreateElement();
            m_hMiddleContainer = gui.CreateElement();
            m_hRightContainer = gui.CreateElement();


            gui.SetElementParent(m_hLeftContainer,   hRootElement);
            gui.SetElementWidth(m_hLeftContainer, 256U);
            gui.SetElementHeightRatio(m_hLeftContainer, 1.0f);
            gui.SetElementMarginRight(m_hLeftContainer, 2);
            
            gui.SetElementParent(m_hMiddleContainer, hRootElement);
            gui.SetElementSizeRatio(m_hMiddleContainer, 1.0f, 1.0f);

            gui.SetElementParent(m_hRightContainer,  hRootElement);
            gui.SetElementWidth(m_hRightContainer, 0U);
            gui.SetElementHeightRatio(m_hRightContainer, 1.0f);

            

            // The asset explorer.
            m_pAssetExplorer = new EditorAssetExplorer(editor);
            gui.SetElementParent(m_pAssetExplorer->GetRootGUIElement(), m_hLeftContainer);


            // The tab group.
            EditorTabGroup* pDefaultTabGroup = new EditorTabGroup(editor);
            m_tabGroups.PushBack(pDefaultTabGroup);

            gui.SetElementParent(pDefaultTabGroup->GetRootGUIElement(), m_hMiddleContainer);
            //gui.SetElementMarginTop(pDefaultTabGroup->GetRootGUIElement(), 8);
        }
    }

    EditorBody::~EditorBody()
    {
    }
}
