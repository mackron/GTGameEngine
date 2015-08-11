// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorTabGroup.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorTabGroup::EditorTabGroup(Editor &editor)
        : EditorControl(editor),
          m_tabBar(editor),
          m_tabPages(),
          m_hTabPageContainer(NULL)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();
            const EditorTheme &theme = editor.GetTheme();

            gui.EnableElementChildHeightFlexing(hRootElement);
            gui.SetElementSizeRatio(hRootElement, 1.0f, 1.0f);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Vertical);

            // Tab bar.
            gui.SetElementParent(m_tabBar.GetRootGUIElement(), hRootElement);

            // Tab page container.
            m_hTabPageContainer = gui.CreateElement();
            gui.SetElementParent(m_hTabPageContainer, hRootElement);
            gui.SetElementSizeRatio(m_hTabPageContainer, 1.0f, 1.0f);
            gui.SetElementBorder(m_hTabPageContainer, 1, GTLib::Colour(0.4f, 0.4f, 0.4f));
            gui.SetElementBorderTopWidth(m_hTabPageContainer, 0);
            gui.SetElementBackgroundColor(m_hTabPageContainer, theme.backgroundMid);
            gui.SetElementPadding(m_hTabPageContainer, 2);


            // Hide the tab bar and page container by default. This will be made visible when a tab is added.
            gui.HideElement(m_tabBar.GetRootGUIElement());
            gui.HideElement(m_hTabPageContainer);
        }
    }

    EditorTabGroup::~EditorTabGroup()
    {
        this->CloseAllTabs();
    }


    EditorTab* EditorTabGroup::CreateTab(const char* title)
    {
        auto pTab = m_tabBar.CreateTab(title);
        if (pTab != nullptr)
        {
            auto pTabPage = new EditorTabPage(this->GetEditor());
            m_tabPages.Add(pTab, pTabPage);

            this->GetGUI().SetElementParent(pTabPage->GetRootGUIElement(), m_hTabPageContainer);

            // Make sure the tab bar and page container are shown.
            this->GetGUI().ShowElement(m_tabBar.GetRootGUIElement());
            this->GetGUI().ShowElement(m_hTabPageContainer);
        }

        return pTab;
    }

    void EditorTabGroup::CloseTab(EditorTab* pTab)
    {
        if (m_tabBar.IsTabOnThisBar(pTab))
        {
            // Remove the tab first.
            m_tabBar.DeleteTab(pTab);

            // Now delete the page.
            auto iTabPage = m_tabPages.Find(pTab);
            if (iTabPage != nullptr)
            {
                delete iTabPage->value;
                m_tabPages.RemoveByIndex(iTabPage->index);
            }


            // If there are no more tabs, hide the page container.
            if (m_tabBar.GetTabCount() == 0)
            {
                this->GetGUI().HideElement(m_tabBar.GetRootGUIElement());
                this->GetGUI().HideElement(m_hTabPageContainer);
            }
        }
    }

    void EditorTabGroup::CloseAllTabs()
    {
        // It's a bit more efficient to close tabs from right to left.
        while (m_tabBar.GetTabCount() > 0)
        {
            this->CloseTab(m_tabBar.GetTabByIndex(m_tabBar.GetTabCount() - 1));
        }
    }

    void EditorTabGroup::ActivateTab(EditorTab* pTab)
    {
        this->DeactivateActiveTab();

        // Tab bar.
        m_tabBar.ActivateTab(pTab);

        // Tab page.
        auto pTabPage = this->GetTabPage(pTab);
        if (pTabPage != nullptr)
        {
            this->GetGUI().ShowElement(pTabPage->GetRootGUIElement());
        }
    }

    void EditorTabGroup::ActivateNeighbouringTab(EditorTab* pTab)
    {
        auto pNeighbouringTab = this->GetNeighbouringTab(pTab);
        if (pNeighbouringTab != nullptr)
        {
            this->ActivateTab(pNeighbouringTab);
        }
    }

    void EditorTabGroup::DeactivateActiveTab()
    {
        auto pActiveTab = this->GetActiveTab();
        if (pActiveTab != nullptr)
        {
            m_tabBar.DeactivateActiveTab();

            auto pTabPage = this->GetTabPage(pActiveTab);
            if (pTabPage != nullptr)
            {
                this->GetGUI().HideElement(pTabPage->GetRootGUIElement());
            }
        }
    }

    EditorTab* EditorTabGroup::GetActiveTab() const
    {
        return m_tabBar.GetActiveTab();
    }

    EditorTab* EditorTabGroup::GetNeighbouringTab(EditorTab* pTab) const
    {
        return m_tabBar.GetNeighbouringTab(pTab);
    }

    bool EditorTabGroup::IsTabInThisGroup(EditorTab* pTab) const
    {
        return m_tabBar.IsTabOnThisBar(pTab);
    }


    EditorTabPage* EditorTabGroup::GetTabPage(EditorTab* pTab) const
    {
        auto iTabPage = m_tabPages.Find(pTab);
        if (iTabPage != nullptr)
        {
            return iTabPage->value;
        }

        return nullptr;
    }
}
