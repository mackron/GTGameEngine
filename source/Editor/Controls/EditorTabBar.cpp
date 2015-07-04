// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorTabBar.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorTabBar::EditorTabBar(Editor &editor)
        : EditorControl(editor),
          m_tabs(),
          m_pActiveTab(nullptr),
          m_hTabContainer(NULL), m_hDropDownContainer(NULL),
          m_tabEventHandler(this)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            GUIContext &gui = editor.GetGUI();

            //gui.SetElementHeight(hRootElement, 28U);
            gui.SetElementHeightToChildren(hRootElement);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementBorderBottom(hRootElement, 1, GTLib::Colour(0.4f, 0.4f, 0.4f));
            gui.EnableElementChildWidthFlexing(hRootElement);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.SetElementFont(hRootElement, "Segoe UI", FontWeight_Medium, FontSlant_None, 12);

            // We have two child elements here. One that is the container for the tabs themselves, and the other that is for the
            // drop-down button that shows the alphabetical list of tabs.
            m_hTabContainer = gui.CreateElement();
            gui.SetElementParent(m_hTabContainer, hRootElement);
            gui.SetElementChildAxis(m_hTabContainer, ChildAxis_Horizontal);
            gui.SetElementSizeRatio(m_hTabContainer, 1, 1);
            gui.SetElementHeightToChildren(m_hTabContainer);
            gui.SetElementMarginLeft(m_hTabContainer, 0);


            // The drop-down container.
            m_hDropDownContainer = gui.CreateElement();
            gui.SetElementParent(m_hDropDownContainer, hRootElement);

            gui.SetElementPadding(m_hDropDownContainer, 8);
            gui.SetElementPaddingTop(m_hDropDownContainer, 0);
            gui.SetElementPaddingBottom(m_hDropDownContainer, 0);

            gui.SetElementWidthToChildren(m_hDropDownContainer);
            gui.SetElementHeightRatio(m_hDropDownContainer, 1.0f);
            gui.SetElementMinWidthToChildren(m_hDropDownContainer);
            gui.SetElementMinHeightToChildren(m_hDropDownContainer);

            gui.SetElementVerticalAlign(m_hDropDownContainer, VerticalAlign_Center);
            gui.SetElementHorizontalAlign(m_hDropDownContainer, HorizontalAlign_Center);

            gui.SetElementText(m_hDropDownContainer, "\xE2\x8B\xAF");   // <-- Midline ellipsis (U+22EF)
            gui.SetElementTextColor(m_hDropDownContainer, GTLib::Colour(0.8f, 0.8f, 0.8f));




        }
    }


    EditorTab* EditorTabBar::CreateTab(const char* text)
    {
        EditorTab* pTab = new EditorTab(this->GetEditor());
        pTab->SetText(text);

        this->GetGUI().PrependChildElement(m_hTabContainer, pTab->GetRootGUIElement());
        this->GetGUI().AttachLocalEventHandler(pTab->GetRootGUIElement(), m_tabEventHandler);

        m_tabs.PushBack(pTab);
        return pTab;
    }

    void EditorTabBar::DeleteTab(EditorTab* pTab)
    {
        if (pTab != nullptr)
        {
            size_t index;
            if (m_tabs.FindFirstIndexOf(pTab, index))
            {
                m_tabs.RemoveFirstOccuranceOf(pTab);
            }

            delete pTab;
        }
    }

    EditorTab* EditorTabBar::GetTabByGUIElement(HGUIElement hElement) const
    {
        for (size_t i = 0; i < m_tabs.GetCount(); ++i)
        {
            auto pTab = m_tabs[i];
            assert(pTab != nullptr);
            {
                if (pTab->GetRootGUIElement() == hElement)
                {
                    return pTab;
                }
            }
        }

        return nullptr;
    }


    EditorTab* EditorTabBar::GetActiveTab() const
    {
        return m_pActiveTab;
    }

    void EditorTabBar::ActivateTab(EditorTab* pTab)
    {
        this->DeactivateTab();

        if (pTab != nullptr)
        {
            m_pActiveTab = pTab;
            m_pActiveTab->ApplyActivatedStyle();
        }
    }

    void EditorTabBar::DeactivateTab()
    {
        if (m_pActiveTab != nullptr)
        {
            m_pActiveTab->ApplyDefaultStyle();
            m_pActiveTab = nullptr;
        }
    }






    ///////////////////////////////////////
    // TabEventHandler

    EditorTabBar::TabEventHandler::TabEventHandler(EditorTabBar* pTabBar)
        : m_pTabBar(pTabBar)
    {
    }

    void EditorTabBar::TabEventHandler::OnMouseEnter(GUIContext &gui, HGUIElement hElement)
    {
        auto pTab = m_pTabBar->GetTabByGUIElement(hElement);
        if (pTab != nullptr)
        {
            pTab->ApplyHoveredStyle();
        }
    }

    void EditorTabBar::TabEventHandler::OnMouseLeave(GUIContext &gui, HGUIElement hElement)
    {
        auto pTab = m_pTabBar->GetTabByGUIElement(hElement);
        if (pTab != nullptr)
        {
            if (pTab != m_pTabBar->GetActiveTab())
            {
                pTab->ApplyDefaultStyle();
            }
        }
    }

    void EditorTabBar::TabEventHandler::OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int, int, int)
    {
        int temp = 0;
    }
}
