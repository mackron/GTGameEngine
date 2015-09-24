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
        }
    }

    EditorBody::~EditorBody()
    {
        for (size_t i = 0; i < m_tabGroups.GetCount(); ++i)
        {
            delete m_tabGroups[i];
        }

        delete m_pAssetExplorer;
    }


    size_t EditorBody::GetTabGroupCount() const
    {
        return m_tabGroups.GetCount();
    }

    EditorTabGroup* EditorBody::GetTabGroupByIndex(size_t index) const
    {
        return m_tabGroups[index];
    }


    bool EditorBody::CloseTab(EditorTab* pTab)
    {
        auto pTabGroup = this->FindTabGroup(pTab);
        if (pTabGroup != nullptr)
        {
            // If the tab is active we want to activate it's neighbour.
            if (pTab == pTabGroup->GetActiveTab())
            {
                pTabGroup->ActivateNeighbouringTab(pTab);
            }
            
            pTabGroup->CloseTab(pTab);


            return true;
        }


        return false;
    }

    bool EditorBody::ActivateTab(EditorTab* pTab)
    {
        auto pTabGroup = this->FindTabGroup(pTab);
        if (pTabGroup != nullptr)
        {
            pTabGroup->ActivateTab(pTab);

            return true;
        }

        return false;
    }

    EditorTabGroup* EditorBody::FindTabGroup(EditorTab* pTab)
    {
        for (size_t iTabGroup = 0; iTabGroup < m_tabGroups.GetCount(); ++iTabGroup)
        {
            auto pTabGroup = m_tabGroups[iTabGroup];
            assert(pTabGroup != nullptr);
            {
                if (pTabGroup->IsTabInThisGroup(pTab))
                {
                    return pTabGroup;
                }
            }
        }

        return nullptr;
    }
}
