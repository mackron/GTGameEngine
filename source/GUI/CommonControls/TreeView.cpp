// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/CommonControls/TreeView.hpp>

namespace GT
{
    TreeView::TreeView(GUIContext &gui, HGUIElement hParent)
        : GUIControl(gui, hParent),
          m_rootItem(gui, nullptr, ""),
          m_hTopContainer(0), m_hBottomContainer(0), m_hRootItemContainer(0),
          m_verticalScrollbar(gui, 0, ScrollbarOrientation::Vertical), m_horizontalScrollbar(gui, 0, ScrollbarOrientation::Horizontal),
          m_hDeadSpace(0),
          m_isMultiSelectionEnabled(false),
          m_onItemSelected(nullptr), m_onItemDeselected(nullptr), m_onItemExpanded(nullptr), m_onItemCollapsed(nullptr), m_onLeafItemPicked(nullptr)
    {
        m_gui.EnableElementChildWidthFlexing(m_hRootElement);
        m_gui.EnableElementChildHeightFlexing(m_hRootElement);
        

        // Top
        {
            m_hTopContainer = m_gui.CreateElement();
            m_gui.SetElementParent(m_hTopContainer, m_hRootElement);
            m_gui.SetElementSizeRatio(m_hTopContainer, 1, 1);
            m_gui.SetElementChildAxis(m_hTopContainer, ChildAxis_Horizontal);
            m_gui.EnableElementChildWidthFlexing(m_hTopContainer);


            // Main content on the left...
            m_hRootItemContainer = m_gui.CreateElement();
            m_gui.SetElementParent(m_hRootItemContainer, m_hTopContainer);
            m_gui.SetElementSizeRatio(m_hRootItemContainer, 1, 1);
            
            m_gui.SetElementParent(m_rootItem.GetRootElement(), m_hRootItemContainer);


            // ... and the vertical scrollbar on the right.
            m_gui.SetElementParent(m_verticalScrollbar.GetRootElement(), m_hTopContainer);
            m_gui.SetElementWidth(m_verticalScrollbar.GetRootElement(), 16U);
            m_gui.SetElementHeightRatio(m_verticalScrollbar.GetRootElement(), 1);
        }


        // Bottom
        {
            m_hBottomContainer = m_gui.CreateElement();
            m_gui.SetElementParent(m_hBottomContainer, m_hRootElement);
            m_gui.SetElementWidthRatio(m_hBottomContainer, 1);
            m_gui.SetElementHeightToChildren(m_hBottomContainer);
            m_gui.SetElementChildAxis(m_hBottomContainer, ChildAxis_Horizontal);
            m_gui.EnableElementChildWidthFlexing(m_hBottomContainer);


            // Horizontal scrollbar on the left...
            m_gui.SetElementParent(m_horizontalScrollbar.GetRootElement(), m_hBottomContainer);
            m_gui.SetElementWidthRatio(m_horizontalScrollbar.GetRootElement(), 1);
            m_gui.SetElementHeight(m_horizontalScrollbar.GetRootElement(), 16U);


            // ... and dead space on the right.
            m_hDeadSpace = m_gui.CreateElement();
            m_gui.SetElementParent(m_hDeadSpace, m_hBottomContainer);
            m_gui.SetElementSize(m_hDeadSpace, 16U, 16U);
        }


        // We need to make a few changes the root item since it's a special one.
        m_gui.HideElement(m_rootItem.GetTitleElement());
        m_rootItem.DisableSelection();
        m_rootItem.SetChildrenIndentation(0);
        m_rootItem.Expand();

        // When the root item changes size, we need to refresh scrollbars.
        m_gui.OnElementSize(m_rootItem.GetRootElement(), [&](unsigned int, unsigned int) {
            this->RefreshScrollbars();
        });

        // We also need to refresh scrollbars when the container element for the root item changes size.
        m_gui.OnElementSize(m_hRootItemContainer, [&](unsigned int, unsigned int) {
            this->RefreshScrollbars();
        });


        // Scrolling.
        m_verticalScrollbar.OnScroll([&](int scrollPos) {
            m_gui.SetElementInnerOffsetY(m_hRootItemContainer, float(-scrollPos * this->GetFirstItemHeight()));
        });
        m_horizontalScrollbar.OnScroll([&](int scrollPos) {
            m_gui.SetElementInnerOffsetX(m_hRootItemContainer, float(-scrollPos));
        });
    }

    TreeView::~TreeView()
    {
    }


    TreeViewItem* TreeView::CreateAndInsertItem(TreeViewItem* pParent, const char* text, unsigned int insertIndex)
    {
        if (pParent == nullptr) {
            pParent = &m_rootItem;
        }

        assert(pParent != nullptr);

        TreeViewItem* pItem = pParent->CreateAndInsertChild(text, insertIndex);
        if (pItem != nullptr)
        {
            pItem->OnSelected([&, pItem]() {
                this->OnItemSelected(pItem);
            });
            pItem->OnDeselected([&, pItem]() {
                this->OnItemDeselected(pItem);
            });
            pItem->OnExpanded([&, pItem]() {
                this->OnItemExpanded(pItem);
            });
            pItem->OnCollapsed([&, pItem]() {
                this->OnItemCollapsed(pItem);
            });
            pItem->OnLeafPicked([&, pItem]() {
                this->OnLeafItemPicked(pItem);
            });
        }

        return pItem;
    }

    TreeViewItem* TreeView::CreateAndAppendItem(TreeViewItem* pParent, const char* text)
    {
        if (pParent == nullptr) {
            pParent = &m_rootItem;
        }

        assert(pParent != nullptr);
        return this->CreateAndInsertItem(pParent, text, pParent->GetChildCount());
    }

    void TreeView::DeleteItem(TreeViewItem* pItem)
    {
        assert(pItem != &m_rootItem);

        delete pItem;
    }



    void TreeView::SelectItem(TreeViewItem* pItem)
    {
        pItem->Select();
    }

    void TreeView::DeselectItem(TreeViewItem* pItem)
    {
        pItem->Deselect();
    }

    void TreeView::DeselectAllItems()
    {
        this->DeselectAllItemsExcept(nullptr);
    }

    void TreeView::DeselectAllItemsExcept(TreeViewItem* pItemToExclude)
    {
        GTLib::Vector<TreeViewItem*> selectedItems(m_selectedItems);
        for (size_t i = 0; i < selectedItems.GetCount(); ++i)
        {
            if (selectedItems[i] != pItemToExclude) {
                this->DeselectItem(selectedItems[i]);
            }
        }
    }


    void TreeView::OnItemSelected(OnItemSelectedProc proc)
    {
        m_onItemSelected = proc;
    }

    void TreeView::OnItemDeselected(OnItemDeselectedProc proc)
    {
        m_onItemDeselected = proc;
    }

    void TreeView::OnItemExpanded(OnItemExpandedProc proc)
    {
        m_onItemExpanded = proc;
    }

    void TreeView::OnItemCollapsed(OnItemCollapsedProc proc)
    {
        m_onItemCollapsed = proc;
    }

    void TreeView::OnLeafItemPicked(OnLeafItemPickedProc proc)
    {
        m_onLeafItemPicked = proc;
    }


    void TreeView::OnItemSelected(TreeViewItem* pItem)
    {
        // If multi-select is disabled, deselect anything that is already selected.
        if (!m_isMultiSelectionEnabled) {
            this->DeselectAllItems();
        }


        assert(!m_selectedItems.Exists(pItem));
        m_selectedItems.PushBack(pItem);

        if (m_onItemSelected != nullptr) {
            m_onItemSelected(pItem);
        }
    }

    void TreeView::OnItemDeselected(TreeViewItem* pItem)
    {
        m_selectedItems.RemoveFirstOccuranceOf(pItem);

        if (m_onItemDeselected != nullptr) {
            m_onItemDeselected(pItem);
        }
    }

    void TreeView::OnItemExpanded(TreeViewItem* pItem)
    {
        if (m_onItemExpanded != nullptr) {
            m_onItemExpanded(pItem);
        }
    }
    void TreeView::OnItemCollapsed(TreeViewItem* pItem)
    {
        if (m_onItemCollapsed != nullptr) {
            m_onItemCollapsed(pItem);
        }
    }
    void TreeView::OnLeafItemPicked(TreeViewItem* pItem)
    {
        if (m_onLeafItemPicked != nullptr) {
            m_onLeafItemPicked(pItem);
        }
    }


    void TreeView::RefreshScrollbars()
    {
        // We just use simple settings for now, but later we'll do this proeprly where we scroll at notches equal to the size of one line of text
        // so we can have cleaner scrolls with the mouse wheel.


        // Vertical.
        int itemHeight = this->GetFirstItemHeight();
        int pageHeight = int(m_gui.GetElementHeight(m_hRootItemContainer)) / itemHeight;
        int itemCount  = int(m_rootItem.CountVisibleChildren());
        m_verticalScrollbar.SetPageSize(pageHeight);
        m_verticalScrollbar.SetRange(0, itemCount + pageHeight - 1 - 1);    // -1 because the range is zero based and inclusive and -1 because we want to see the last item.


        // Horizontal.
        int pageWidth    = int(m_gui.GetElementWidth(m_hRootItemContainer));
        int contentWidth = int(m_gui.GetElementWidth(m_rootItem.GetRootElement()));
        m_horizontalScrollbar.SetPageSize(pageWidth);
        m_horizontalScrollbar.SetRange(0, contentWidth - 1);                // -1 because the range is zero based and inclusive.
    }

    int TreeView::GetFirstItemHeight() const
    {
        if (m_rootItem.GetChildCount() > 0) {
            return int(m_gui.GetElementHeight(m_rootItem.GetChildByIndex(0)->GetTitleElement()));
        }

        return 21;
    }
}
