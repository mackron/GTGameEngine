// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorAssetExplorer
#define GT_EditorAssetExplorer

#include "EditorScrollableControl.hpp"
#include "EditorAssetExplorerItem.hpp"

namespace GT
{
    /// The asset explorer control.
    class EditorAssetExplorer : public EditorScrollableControl
    {
    public:

        EditorAssetExplorer(Editor &editor);
        ~EditorAssetExplorer();



        /// Adds a base directory.
        ///
        /// @param absolutePath [in] The absolute path of the base directory.
        /// @param displayName  [in] The display name of the base directory.
        void AddBaseDirectory(const char* absolutePath, const char* displayName);

        /// Adds an item by it's absolute path.
        ///
        /// @remarks
        ///     This will create the necessary hierarchy.
        ///     @par
        ///     This needs to be a descendant of a base directory.
        //void AddItem(const char* absolutePath);


        /// Determines whether or not the given path is a base directory.
        bool IsBaseDirectory(const char* absolutePath) const;


        /// Selects the given item based on the input from the mouse and keyboard.
        ///
        /// @remarks
        ///     This will select the item based on which keys are currently being held down on the keyboard, namely CTRL and Shift.
        void SelectItemFromMouseInput(EditorAssetExplorerItem &item);

        /// Selects the given item.
        void SelectItem(EditorAssetExplorerItem &item);

        /// Deselects the given item.
        void DeselectItem(EditorAssetExplorerItem &item);

        /// Deselects every selected item.
        void DeselectAllItems();


        /// Scrolls.
        void ScrollY(int delta);

        /// Scrolls such that the item at the given index is the top item.


    private:

        /// The list of base directories.
        GTLib::Vector<EditorAssetExplorerItem*> m_baseItems;

        /// The list of selected items.
        GTLib::Vector<EditorAssetExplorerItem*> m_selectedItems;


        struct ExplorerEventHandler : public GUIEventHandler
        {
            /// Constructor.
            ExplorerEventHandler(EditorAssetExplorer &explorer);

            /// @copydoc GUIEventHandler::OnMouseWheel()
            void OnMouseWheel(GUIContext &context, HGUIElement, int delta, int, int);


            EditorAssetExplorer &m_explorer;


        private:    // No copying.
            ExplorerEventHandler(const ExplorerEventHandler &);
            ExplorerEventHandler & operator=(const ExplorerEventHandler &);
        };

        ExplorerEventHandler m_eventHandler;
    };
}

#endif
