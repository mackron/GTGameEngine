// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/CommonControls/TreeViewItem.hpp>

namespace GT
{
    static const char* g_ArrowFacingRightString = "\xE2\x96\xB6";       // U+25B6
    //static const char* g_ArrowFacingDownString  = "\xE2\x96\xBC";       // U+25BC. Straight down.
    static const char* g_ArrowFacingDownString  = "\xE2\x97\xA2";       // U+25E2. Diagonal (Windows' style)

    TreeViewItem::TreeViewItem(GUIContext &gui, TreeViewItem *pParent, const char* text)
        : GUIControl(gui, 0),
          m_pParent(pParent), m_children(),
          m_isExpanded(false), m_isSelected(false), m_isSelectionEnabled(true),
          m_localChildrenIndentation(16),
          m_onSelected(nullptr), m_onDeselected(nullptr), m_onExpanded(nullptr), m_onCollapsed(nullptr), m_onLeafPicked(nullptr),
          m_userDataSize(0), m_pUserData(nullptr)
    {
        m_gui.SetElementMinWidthRatio(m_hRootElement, 1);
        m_gui.SetElementWidthToChildren(m_hRootElement);
        m_gui.SetElementHeightToChildren(m_hRootElement);

        //m_gui.SetElementBorder(m_hRootElement, 1, GT::Color::Black);    // TEMP


        // Title.
        {
            m_hTitle = m_gui.CreateElement();
            m_gui.SetElementParent(m_hTitle, m_hRootElement);
            m_gui.SetElementChildAxis(m_hTitle, ChildAxis_Horizontal);
            m_gui.SetElementMinWidthRatio(m_hTitle, 1);
            m_gui.SetElementWidthToChildren(m_hTitle);
            m_gui.SetElementHeightToChildren(m_hTitle);
            m_gui.SetElementVerticalAlign(m_hTitle, VerticalAlign_Center);
            m_gui.SetElementPaddingLeft(m_hTitle, 4 + this->GetAbsoluteIndentation());
            m_gui.SetElementPaddingTop(m_hTitle, 1);
            m_gui.SetElementPaddingBottom(m_hTitle, 1);

            m_gui.OnElementMouseEnter(m_hTitle, [&]() {
                this->EnableHoveredStyle();
            });
            m_gui.OnElementMouseLeave(m_hTitle, [&]() {
                this->DisableHoveredStyle();
            });
            m_gui.OnElementMouseButtonPressed(m_hTitle, [&](int button, int, int) {
                if (button == 1) {      // Left button
                    this->Select();
                }
            });
            m_gui.OnElementMouseButtonDoubleClicked(m_hTitle, [&](int button, int, int) {
                if (button == 1) {      // Left button
                    if (this->HasChildren()) {
                        this->ToggleExpand();
                    } else {
                        if (m_onLeafPicked != nullptr) {
                            m_onLeafPicked();
                        }
                    }
                }
            });

            //m_gui.SetElementBorder(m_hTitle, 1, GT::Color::Red);    // TEMP

            // Title Arrow
            {
                m_hTitleArrow = m_gui.CreateElement();
                m_gui.SetElementParent(m_hTitleArrow, m_hTitle);
                m_gui.SetElementText(m_hTitleArrow, "");
                m_gui.SetElementTextColor(m_hTitleArrow, GT::Color(0.8f, 0.8f, 0.8f));
                m_gui.SetElementFont(m_hTitleArrow, "Segoe UI Symbol", FontWeight_Medium, FontSlant_None, 9);
                m_gui.SetElementHeightToChildren(m_hTitleArrow);
                m_gui.SetElementWidth(m_hTitleArrow, 16U);

                m_gui.OnElementMouseEnter(m_hTitleArrow, [&]() {
                    m_gui.SetElementTextColor(m_hTitleArrow, GT::Color::White);
                });
                m_gui.OnElementMouseLeave(m_hTitleArrow, [&]() {
                    m_gui.SetElementTextColor(m_hTitleArrow, GT::Color(0.8f, 0.8f, 0.8f));
                });
                m_gui.OnElementMouseButtonPressed(m_hTitleArrow, [&](int button, int, int) {
                    if (button == 1) {    // Left button
                        this->ToggleExpand();
                    }
                });
            }


            // Title Icon
            {
                m_hTitleIcon = m_gui.CreateElement();
                m_gui.SetElementParent(m_hTitleIcon, m_hTitle);
                m_gui.SetElementHeightToChildren(m_hTitleIcon);
                m_gui.EnableCursorPassThrough(m_hTitleIcon);
            }

        
            // Title Text
            {
                m_hTitleText = m_gui.CreateElement();
                m_gui.SetElementParent(m_hTitleText, m_hTitle);
                m_gui.SetElementSizeToChildren(m_hTitleText);
                m_gui.EnableCursorPassThrough(m_hTitleText);
                m_gui.SetElementPaddingBottom(m_hTitleText, 2);
                m_gui.SetElementText(m_hTitleText, text);
                m_gui.SetElementTextColor(m_hTitleText, GT::Color(0.8f, 0.8f, 0.8f));
            }
        }


        // Children Container.
        {
            m_hChildrenContainer = m_gui.CreateElement();
            m_gui.SetElementParent(m_hChildrenContainer, m_hRootElement);
            m_gui.SetElementMinWidthRatio(m_hChildrenContainer, 1);
            m_gui.SetElementWidthToChildren(m_hChildrenContainer);
            m_gui.SetElementHeightToChildren(m_hChildrenContainer);
            m_gui.HideElement(m_hChildrenContainer);
        }
    }

    TreeViewItem::~TreeViewItem()
    {
        this->DeleteAllChildren();

        m_gui.DeleteElement(m_hChildrenContainer);
        m_gui.DeleteElement(m_hTitleText);
        m_gui.DeleteElement(m_hTitleIcon);
        m_gui.DeleteElement(m_hTitleArrow);

        this->FreeUserData();
    }


    TreeViewItem* TreeViewItem::CreateAndInsertChild(const char* text, unsigned int insertIndex)
    {
        assert(insertIndex <= m_children.GetCount());

        TreeViewItem* pNewItem = new TreeViewItem(m_gui, this, text);

        TreeViewItem* pItemToInsertAfter = nullptr;
        if (insertIndex > 0)
        {
            pItemToInsertAfter = m_children[insertIndex - 1];
            assert(pItemToInsertAfter != nullptr);

            m_gui.AppendSiblingElement(pItemToInsertAfter->GetRootElement(), pNewItem->GetRootElement());
        }
        else
        {
            // It's the first in the list. The GUI element is prepended to the child container.
            m_gui.PrependChildElement(m_hChildrenContainer, pNewItem->GetRootElement());
        }

        m_children.InsertAt(pNewItem, insertIndex);


        // Now that we have children we can show the arrow.
        if (m_children.GetCount() == 1) {
            m_gui.SetElementText(m_hTitleArrow, g_ArrowFacingRightString);
        }
        

        return pNewItem;
    }

    void TreeViewItem::DeleteAllChildren()
    {
        for (size_t i = 0; i < m_children.GetCount(); ++i)
        {
            delete m_children[i];
        }
        m_children.Clear();


        // Remove the arrow.
        m_gui.SetElementText(m_hTitleArrow, g_ArrowFacingRightString);
    }


    bool TreeViewItem::HasChildren() const
    {
        return m_children.GetCount() > 0;
    }

    unsigned int TreeViewItem::GetChildCount() const
    {
        return static_cast<unsigned int>(m_children.GetCount());
    }

    TreeViewItem* TreeViewItem::GetChildByIndex(unsigned int index) const
    {
        return m_children[index];
    }


    void TreeViewItem::Expand()
    {
        if (!this->IsExpanded())
        {
            if (this->HasChildren()) {
                m_gui.SetElementText(m_hTitleArrow, g_ArrowFacingDownString);
            }

            m_gui.ShowElement(m_hChildrenContainer);

            if (m_onExpanded != nullptr) {
                m_onExpanded();
            }

            m_isExpanded = true;
        }
    }

    void TreeViewItem::Collapse()
    {
        if (this->IsExpanded())
        {
            if (this->HasChildren()) {
                m_gui.SetElementText(m_hTitleArrow, g_ArrowFacingRightString);
            }

            m_gui.HideElement(m_hChildrenContainer);

            if (m_onCollapsed != nullptr) {
                m_onCollapsed();
            }

            m_isExpanded = false;
        }
    }

    void TreeViewItem::ToggleExpand()
    {
        if (!this->IsExpanded())
        {
            this->Expand();
        }
        else
        {
            this->Collapse();
        }
    }

    bool TreeViewItem::IsExpanded() const
    {
        return m_isExpanded;
    }


    void TreeViewItem::Select()
    {
        if (!this->IsSelected() && this->IsSelectionEnabled())
        {
            this->EnabledSelectedStyle();

            if (m_onSelected != nullptr) {
                m_onSelected();
            }

            m_isSelected = true;
        }
    }

    void TreeViewItem::Deselect()
    {
        if (this->IsSelected())
        {
            this->DisableSelectedStyle();

            if (m_onDeselected != nullptr) {
                m_onDeselected();
            }

            m_isSelected = false;
        }
    }

    void TreeViewItem::ToggleSelect()
    {
        if (!this->IsSelected())
        {
            this->Select();
        }
        else
        {
            this->Select();
        }
    }

    bool TreeViewItem::IsSelected() const
    {
        return m_isSelected;
    }


    void TreeViewItem::DisableSelection()
    {
        if (m_isSelectionEnabled)
        {
            if (this->IsSelected()) {
                this->Deselect();
            }

            m_isSelectionEnabled = false;
        }
    }

    void TreeViewItem::EnableSelection()
    {
        if (!m_isSelectionEnabled) {
            m_isSelectionEnabled = true;
        }
    }



    void TreeViewItem::EnableHoveredStyle()
    {
        if (!this->IsSelected())
        {
            m_gui.SetElementBackgroundColor(m_hTitle, GT::Color(0.35f, 0.35f, 0.35f));
        }
    }

    void TreeViewItem::DisableHoveredStyle()
    {
        if (!this->IsSelected())
        {
            m_gui.SetElementBackgroundColor(m_hTitle, GT::Color(0, 0, 0, 0));
        }
    }


    void TreeViewItem::EnabledSelectedStyle()
    {
        m_gui.SetElementBackgroundColor(m_hTitle, GT::Color(0.4f, 0.4f, 0.4f));
    }

    void TreeViewItem::DisableSelectedStyle()
    {
        m_gui.SetElementBackgroundColor(m_hTitle, GT::Color(0, 0, 0, 0));
    }


    unsigned int TreeViewItem::GetDepth() const
    {
        if (m_pParent != nullptr)
        {
            return m_pParent->GetDepth() + 1;
        }

        return 0;
    }

    unsigned int TreeViewItem::GetAbsoluteIndentation() const
    {
        if (m_pParent != nullptr) {
            return m_pParent->GetAbsoluteChildrenIndentation();
        }

        return 0;
    }

    unsigned int TreeViewItem::GetAbsoluteChildrenIndentation() const
    {
        if (m_pParent != nullptr)
        {
            return m_pParent->GetAbsoluteChildrenIndentation() + m_localChildrenIndentation;
        }

        return m_localChildrenIndentation;
    }

    unsigned int TreeViewItem::GetChildrenIndentation() const
    {
        return m_localChildrenIndentation;
    }

    void TreeViewItem::SetChildrenIndentation(unsigned int depth)
    {
        m_localChildrenIndentation = depth;
    }


    void* TreeViewItem::AllocateUserData(unsigned int sizeInBytes)
    {
        // Free the old data first so we don't leak.
        this->FreeUserData();

        m_pUserData = malloc(sizeInBytes);
        return m_pUserData;
    }

    void TreeViewItem::FreeUserData()
    {
        free(m_pUserData);
    }

    void* TreeViewItem::GetUserData() const
    {
        return m_pUserData;
    }


    void TreeViewItem::OnSelected(OnSelectedProc proc)
    {
        m_onSelected = proc;
    }
    void TreeViewItem::OnDeselected(OnDeselectedProc proc)
    {
        m_onDeselected = proc;
    }
    void TreeViewItem::OnExpanded(OnExpandedProc proc)
    {
        m_onExpanded = proc;
    }
    void TreeViewItem::OnCollapsed(OnCollapsedProc proc)
    {
        m_onCollapsed = proc;
    }
    void TreeViewItem::OnLeafPicked(OnLeafPickedProc proc)
    {
        m_onLeafPicked = proc;
    }
}
