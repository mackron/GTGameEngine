// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_TreeViewItem
#define GT_TreeViewItem

#include "GUIControl.hpp"

namespace GT
{
    class TreeViewItem : public GUIControl
    {
    public:

        typedef std::function<void ()> OnSelectedProc;
        typedef std::function<void ()> OnDeselectedProc;
        typedef std::function<void ()> OnExpandedProc;
        typedef std::function<void ()> OnCollapsedProc;
        typedef std::function<void ()> OnLeafPickedProc;



        /// Constructor
        TreeViewItem(GUIContext &gui, TreeViewItem *pParent, const char* text);

        /// Destructor.
        ~TreeViewItem();


        /// Retrieves a reference to the root title element.
        HGUIElement GetTitleElement() const { return m_hTitle; }

        /// Retrieves the title's arrow element.
        HGUIElement GetArrowElement() const { return m_hTitleArrow; }

        /// Retrieves the title's icon element.
        HGUIElement GetIconElement() const { return m_hTitleIcon; }

        /// Retrieves the title's text element.
        HGUIElement GetTextElement() const { return m_hTitleText; }

        /// Retrieves a reference to the children container element.
        HGUIElement GetChildrenContainer() const { return m_hChildrenContainer; }



        /// Creates and inserts a child item at the given index.
        ///
        /// @param text        [in] The text of the child item.
        /// @param insertIndex [in] The index to insert the child.
        TreeViewItem* CreateAndInsertChild(const char* text, unsigned int insertIndex);

        /// Deletes every child element.
        void DeleteAllChildren();



        /// Determines whether or not the element has children.
        bool HasChildren() const;

        /// Retrieves the number of children for this element.
        unsigned int GetChildCount() const;

        /// Retrieves a pointer to the child item at the given index.
        TreeViewItem* GetChildByIndex(unsigned int index) const;

        /// Recursively counts the number of children that are visible on the given element.
        unsigned int CountVisibleChildren() const;


        /// Expands the item.
        void Expand();

        /// Collapses the item.
        void Collapse();

        /// Toggles the expand/collapse state of the item.
        void ToggleExpand();

        /// Determines whether or not the item is expanded.
        bool IsExpanded() const;


        /// Selects the item.
        void Select();

        /// Deselects the item.
        void Deselect();

        /// Toggles the selection state.
        void ToggleSelect();

        /// Determines whether or not the item is selected.
        bool IsSelected() const;

        /// Disables selection.
        ///
        /// @remarks
        ///     If the item is selected, it will be deselected.
        void DisableSelection();

        /// Enables selection.
        void EnableSelection();

        /// Determines whether or not selection is enabled.
        bool IsSelectionEnabled() const { return m_isSelectionEnabled; }



        /// Enables the hovered styling.
        void EnableHoveredStyle();

        /// Disables the hovered styling.
        void DisableHoveredStyle();


        /// Enables the selected styling.
        void EnabledSelectedStyle();

        /// Disables the selected styling.
        void DisableSelectedStyle();


        /// Retrieves the depth of the item so that the indentation can be calculated correctly.
        unsigned int GetDepth() const;

        /// Retrieves the absolute indentation amount to apply to this element.
        unsigned int GetAbsoluteIndentation() const;

        /// Retrieves the absolute indentation amount to apply to children of this element.
        ///
        /// @remarks
        ///     This works by recursively summing the results of GetChildrenIndentation().
        unsigned int GetAbsoluteChildrenIndentation() const;

        /// Retrieves the indentation amount to apply to children of this element.
        unsigned int GetChildrenIndentation() const;

        /// Sets the amount of indentation to apply to children, relative to this element.
        void SetChildrenIndentation(unsigned int depth);


        /// Allocates a buffer for the user data.
        ///
        /// @remarks
        ///     This is freed when the item is deleted.
        void* AllocateUserData(unsigned int sizeInBytes);

        /// Frees the user data buffer.
        void FreeUserData();

        /// Retrieves a pointer to the user data buffer.
        void* GetUserData() const;


        /// Registers the function to call when the item is selected.
        void OnSelected(OnSelectedProc proc);

        /// Registers the function to call when the item is deselected.
        void OnDeselected(OnDeselectedProc proc);

        /// Registers the function to call when the item is expanded.
        void OnExpanded(OnExpandedProc proc);

        /// Registers the function to call when the item is collapsed.
        void OnCollapsed(OnCollapsedProc proc);

        /// Registers the function to call when an item with no children is picked (double clicked).
        void OnLeafPicked(OnLeafPickedProc proc);


    private:

        /// A pointer to the parent element.
        TreeViewItem *m_pParent;

        /// The list of child items.
        GTLib::Vector<TreeViewItem*> m_children;


        // The tree view is divided into two main parts - the title and the container for children.

        /// The root element of the title.
        HGUIElement m_hTitle;

        /// The element containing the arrow for expanding and collapsing children.
        HGUIElement m_hTitleArrow;

        /// The element containing the icon.
        HGUIElement m_hTitleIcon;

        /// The element containing the title text.
        HGUIElement m_hTitleText;

        /// The root element of the children container.
        HGUIElement m_hChildrenContainer;



        /// Keeps track of whether or not the item is expanded.
        bool m_isExpanded;

        /// Keeps track of whether or not the item is selected.
        bool m_isSelected;

        /// Keeps track of whether or not the item is selectable.
        bool m_isSelectionEnabled;


        /// The local indentation amount to apply to children.
        unsigned int m_localChildrenIndentation;



        /// The function to call when the item is selected.
        OnSelectedProc m_onSelected;

        /// The function to call when an item is deselected.
        OnDeselectedProc m_onDeselected;

        /// The function to call when an item is expanded.
        OnExpandedProc m_onExpanded;

        /// The function to call when an item is collapsed.
        OnCollapsedProc m_onCollapsed;

        /// The function to call when an leaf item is picked (double clicked).
        OnLeafPickedProc m_onLeafPicked;


        /// The size of the user data buffer.
        unsigned int m_userDataSize;

        /// A pointer to the buffer for storing user-defined data.
        void* m_pUserData;
    };
}

#endif
