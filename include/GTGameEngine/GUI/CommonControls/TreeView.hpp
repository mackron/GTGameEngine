// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_TreeView
#define GT_TreeView

#include "TreeViewItem.hpp"

namespace GT
{
    class TreeView : public GUIControl
    {
    public:

        typedef std::function<void (TreeViewItem* pItem)> OnItemSelectedProc;
        typedef std::function<void (TreeViewItem* pItem)> OnItemDeselectedProc;
        typedef std::function<void (TreeViewItem* pItem)> OnItemExpandedProc;
        typedef std::function<void (TreeViewItem* pItem)> OnItemCollapsedProc;
        typedef std::function<void (TreeViewItem* pItem)> OnLeafItemPickedProc;


        /// Constructor
        TreeView(GUIContext &gui, HGUIElement hParent = 0);

        /// Destructor.
        ~TreeView();


        /// Retrieves a reference to the root item.
        TreeViewItem & GetRootItem() { return m_rootItem; }


        /// Creates and inserts a new item.
        TreeViewItem* CreateAndInsertItem(TreeViewItem* pParent, const char* text, unsigned int insertIndex);
        TreeViewItem* CreateAndAppendItem(TreeViewItem* pParent, const char* text);

        /// Deletes the given item.
        void DeleteItem(TreeViewItem* pItem);



        /// Selects the given item.
        ///
        /// @remarks
        ///     This will select the item regardless of whether or not multi-select is enabled.
        void SelectItem(TreeViewItem* pItem);

        /// Deselects the given item.
        void DeselectItem(TreeViewItem* pItem);

        /// Deselects every selected item.
        void DeselectAllItems();

        /// Deselects every selected item except for the specified on.
        void DeselectAllItemsExcept(TreeViewItem* pItemToExclude);


        /// The function to call when an item is selected.
        void OnItemSelected(OnItemSelectedProc proc);

        /// The function to call when an item is deselected.
        void OnItemDeselected(OnItemDeselectedProc proc);

        /// The function to call when an item is expanded.
        void OnItemExpanded(OnItemExpandedProc proc);

        /// The function to call when an item is collapsed.
        void OnItemCollapsed(OnItemCollapsedProc proc);

        /// The function to call when an item without any children is picked (double clicked).
        void OnLeafItemPicked(OnLeafItemPickedProc proc);



    private:

        /// Called when an item is selected.
        void OnItemSelected(TreeViewItem* pItem);

        /// Called when an item is deselected.
        void OnItemDeselected(TreeViewItem* pItem);

        /// Called when an item is expanded.
        void OnItemExpanded(TreeViewItem* pItem);

        /// Called when when an item is collapsed.
        void OnItemCollapsed(TreeViewItem* pItem);

        /// Called when when an item without any children is picked (double clicked).
        void OnLeafItemPicked(TreeViewItem* pItem);


    private:

        /// The root tree view item. All items are children of this item.
        TreeViewItem m_rootItem;

        /// Keeps track of all of the selected items.
        GTLib::Vector<TreeViewItem*> m_selectedItems;

        /// Keeps track of whether or not multi-selection is enabled.
        bool m_isMultiSelectionEnabled;


        /// The function to call when an item is selected.
        OnItemSelectedProc m_onItemSelected;

        /// The function to call when an item is deselected.
        OnItemDeselectedProc m_onItemDeselected;

        /// The function to call when an item is expanded.
        OnItemExpandedProc m_onItemExpanded;

        /// The function to call when an item is collapsed.
        OnItemCollapsedProc m_onItemCollapsed;

        /// The function to call when an leaf item is picked (double clicked).
        OnLeafItemPickedProc m_onLeafItemPicked;
    };
}

#endif
