// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorMenu
#define GT_EditorMenu

#include "EditorPopupControl.hpp"
#include "EditorMenuItem.hpp"
#include "EditorMenuSeparator.hpp"

namespace GT
{
    class EditorMenu : public EditorPopupControl
    {
    public:

        typedef std::function<void ()> OnCloseProc;


        /// Constructor
        EditorMenu(Editor &editor, HWindow hParentWindow);

        /// Destructor.
        ~EditorMenu();


        /// Closes the menu.
        ///
        /// @remarks
        ///     This will hide the window and post OnClose.
        void Close();

        /// Sets the function to call when the menu is closed.
        void OnClose(OnCloseProc proc);


        /// Appends a menu item.
        EditorMenuItem* AppendMenuItem(const char* text, const char* shortcut);

        /// Remove a menu item.
        void RemoveMenuItem(EditorMenuItem* pMenuItem);


        /// Appends a separator to the menu.
        EditorMenuSeparator* AppendSeparator();

        /// Removes a separator.
        void RemoveSeparator(EditorMenuSeparator* pSeparator);


        /// Refreshes the alignment of the items that were created with AppendMenuItem.
        void RefreshMenuItemAlignments();


    private:

        /// The list of menu items that were created by AppendMenuItem(). We need to track these so we can
        /// delete them later.
        GTLib::Vector<EditorMenuItem*> m_menuItems;

        /// The list of menu separators that were created by AppendSeparator(). We need to track these so
        /// we can delete them later.
        GTLib::Vector<EditorMenuSeparator*> m_menuSeparators;


        /// Called when the menu closes itself.
        OnCloseProc m_onClose;
    };
}

#endif
