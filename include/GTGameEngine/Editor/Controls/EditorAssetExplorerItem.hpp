// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorAssetExplorerItem_hpp_
#define __GT_EditorAssetExplorerItem_hpp_

#include "EditorControl.hpp"
#include "../../FileSystem.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    class EditorAssetExplorer;

    // Class representing an item in the asset explorer.
    class EditorAssetExplorerItem : public EditorControl
    {
    public:

        EditorAssetExplorerItem(Editor &editor, EditorAssetExplorerItem* pParentItem, EditorAssetExplorer &explorer, FileSystem &fileSystem, const char* absolutePath, const char* displayName);
        ~EditorAssetExplorerItem();


        /// Retrieves a reference to the asset explorer that owns this item.
        EditorAssetExplorer & GetAssetExplorer() { return m_explorer; }

        /// Retrieves the absolute path of the item.
        const char* GetAbsolutePath() const { return m_absolutePath.c_str(); }

        /// Retrieves a handle to the child container GUI element.
        HGUIElement GetChildContainerGUIElement() const { return m_hChildContainer; }

        /// Determines whether or not the item is a folder.
        bool IsFolder() const { return m_isFolder; }


        /// Creates and inserts a child item.
        ///
        /// The absolute path must be a direct child of this element. If it is not, null will be returned.
        EditorAssetExplorerItem* CreateAndInsertChild(const char* absolutePath);

        /// Deletes every child.
        ///
        /// @remarks
        ///     This is recursive.
        void DeleteAllChildren();


        /// Finds a child by it's absolute path.
        ///
        /// @remarks
        ///     This is not recursive. Use FindDescendantByAbsolutePath() if a recursive search is required.
        EditorAssetExplorerItem* FindChildByAbsolutePath(const char* absolutePath);

        /// Finds a descendant by it's absolute path.
        ///
        /// @remarks
        ///     This is recursive. Use FindChildByAbsolutePath() is recursion is not needed.
        EditorAssetExplorerItem* FindDescendantByAbsolutePath(const char* absolutePath);

        /// Finds the insertion index for a child element based on the given absolute path, in alphabetical order.
        ///
        /// @remarks
        ///     This assumes the internal list is already in alphabetical order.
        ///     @par
        ///     This is not recursive.
        void FindChildAlphabeticalInsertIndex(const char* absolutePath, size_t &insertIndexOut) const;


        /// Expands the item.
        void Expand();

        /// Collapses the item.
        void Collapse();

        /// Toggles the expand/collapse state of the item.
        void ToggleExpand();

        /// Determines whether or not the item is expanded.
        bool IsExpanded() const;

        /// If the item is a folder, toggles the expand/collapse state, and if it's a file opens the file.
        void OpenFileOrToggleExpand();


        /// Selects the item.
        void Select();

        /// Deselects the item.
        void Deselect();

        /// Toggles the selection state.
        void ToggleSelect();

        /// Determines whether or not the item is selected.
        bool IsSelected() const;


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


    private:

        /// Retrieves the index of the file in the children list.
        bool FirstFirstFileIndex(size_t &indexOut) const;



    private:

        /// A pointer to the parent item, if any.
        EditorAssetExplorerItem* m_pParent;

        /// A reference to the asset explorer that owns this item.
        EditorAssetExplorer &m_explorer;

        /// A reference to the file system to use for scanning for children.
        FileSystem &m_fileSystem;

        /// The absolute path of the item.
        GTLib::String m_absolutePath;


        /// Keeps track of whether or not the item is a folder.
        bool m_isFolder;

        /// Keeps track of whether or not the item is expanded.
        bool m_isExpanded;

        /// Keeps track of whether or not the item is selected.
        bool m_isSelected;


        /// The container for the text section.
        HGUIElement m_hTitleContainer;

        /// The container for the arrow icon.
        HGUIElement m_hTitleArrow;

        /// The element containing the icon.
        HGUIElement m_hTitleIcon;

        /// The element containing the title text.
        HGUIElement m_hTitleText;


        /// The container for child items.
        HGUIElement m_hChildContainer;


        /// The list of child element. This is in the order the items are shown in the explorer. Thus, folders are at the top.
        GTLib::Vector<EditorAssetExplorerItem*> m_children;


        struct TitleEventHandler : public GUIEventHandler
        {
            TitleEventHandler(EditorAssetExplorerItem &explorerItem)
                : m_explorerItem(explorerItem)
            {
            }


            void OnMouseEnter(GUIContext &, HGUIElement)
            {
                m_explorerItem.EnableHoveredStyle();
            }

            void OnMouseLeave(GUIContext &, HGUIElement)
            {
                m_explorerItem.DisableHoveredStyle();
            }

            void OnMouseButtonPressed(GUIContext &, HGUIElement, int mouseButton, int, int);

            void OnMouseButtonDoubleClicked(GUIContext &, HGUIElement, int mouseButton, int, int)
            {
                if (mouseButton == 1)   // Left
                {
                    m_explorerItem.OpenFileOrToggleExpand();
                }
            }


            EditorAssetExplorerItem &m_explorerItem;


        private:    // No copying.
            TitleEventHandler(const TitleEventHandler &);
            TitleEventHandler & operator=(const TitleEventHandler &);
        };

        TitleEventHandler m_titleEventHandler;
    };
}

#endif
