// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_TreeView
#define GT_TreeView

#include "TreeViewItem.hpp"
#include "Scrollbar.hpp"

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


        /// Refreshes the scrollbars.
        void RefreshScrollbars();

        /// Retrieves the height of a single item in the tree view. This is used for scrolling.
        int GetFirstItemHeight() const;



    private:

        /// The root tree view item. All items are children of this item.
        TreeViewItem m_rootItem;


        // In order to make room for the scrollbars, the tree view is divided up into four parts. The main content
        // is placed in the top-left section, the vertical scrollbar is in the top-right, and the horizontal scrollbar
        // is placed in the bottom-left section. The bottom-right section is the dead spot between the scrollbars.

        /// The element containing the top elements (the main content and the vertical scrollbar).
        HGUIElement m_hTopContainer;

        /// The element containing the bottom elements (the horizontal scrollbar and the dead space).
        HGUIElement m_hBottomContainer;


        /// The container for the root item.
        HGUIElement m_hRootItemContainer;

        /// The vertical scrollbar.
        Scrollbar m_verticalScrollbar;

        /// The horizontal scrollbar.
        Scrollbar m_horizontalScrollbar;

        /// The dead space between the scrollbars.
        HGUIElement m_hDeadSpace;



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
