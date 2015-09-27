// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GUI/CommonControls/TreeView.hpp>

namespace GT
{
    TreeView::TreeView(GUIContext &gui, HGUIElement hParent)
        : GUIControl(gui, hParent),
          m_rootItem(gui, nullptr, ""),
          m_isMultiSelectionEnabled(false),
          m_onItemSelected(nullptr), m_onItemDeselected(nullptr), m_onItemExpanded(nullptr), m_onItemCollapsed(nullptr), m_onLeafItemPicked(nullptr)
    {
        m_gui.SetElementParent(m_rootItem.GetRootElement(), m_hRootElement);


        // We need to make a few changes the root item since it's a special one.
        m_gui.HideElement(m_rootItem.GetTitleElement());
        m_rootItem.DisableSelection();
        m_rootItem.SetChildrenIndentation(0);
        m_rootItem.Expand();
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

    void TreeView::OnItemExpanded(TreeViewItem * pItem)
    {
        if (m_onItemExpanded != nullptr) {
            m_onItemExpanded(pItem);
        }
    }
    void TreeView::OnItemCollapsed(TreeViewItem * pItem)
    {
        if (m_onItemCollapsed != nullptr) {
            m_onItemCollapsed(pItem);
        }
    }
    void TreeView::OnLeafItemPicked(TreeViewItem * pItem)
    {
        if (m_onLeafItemPicked != nullptr) {
            m_onLeafItemPicked(pItem);
        }
    }
}
