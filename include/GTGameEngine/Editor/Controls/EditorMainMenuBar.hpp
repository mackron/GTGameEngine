// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorMainMenuBarControl_hpp_
#define __GT_EditorMainMenuBarControl_hpp_

#include "EditorMenuBar.hpp"
#include "EditorPopupControl.hpp"

namespace GT
{
    class EditorMainMenuBar : public EditorMenuBar
    {
    public:

        /// Constructor.
        EditorMainMenuBar(Editor &editor, HWindow hParentWindow);

        /// Destructor.
        ~EditorMainMenuBar();



        //////////////////////////////////
        // Inbound Events

        /// @copydoc EditorMenuBar::_OnMouseButtonPressedOnWindow()
        virtual void _OnMouseButtonPressedOnWindow(HWindow hWindow, int button, int xPos, int yPos);


        //////////////////////////////////
        // Events

        /// @copydoc EditorMenuBar::OnButtonActivated()
        virtual void OnButtonActivated(EditorMenuBarButton* pButton);

        /// @copydoc EditorMenuBar::OnButtonDeactivated()
        virtual void OnButtonDeactivated(EditorMenuBarButton* pButton);


    private:

        /// Retrieves the popup control associated with the given button.
        EditorPopupControl* GetButtonMenu(EditorMenuBarButton* pButton) const;

        /// Retrieves a pointer to the popup control of the menu of the current menu.
        EditorPopupControl* GetActiveMenu() const;


    private:

        /// A pointer to the button for the File menu.
        EditorMenuBarButton* m_pFileMenuButton;

        /// A pointer to the button for the Edit menu.
        EditorMenuBarButton* m_pEditMenuButton;

        /// A pointer to the button for the View menu.
        EditorMenuBarButton* m_pViewMenuButton;

        /// A pointer to the button for the Help menu.
        EditorMenuBarButton* m_pHelpMenuButton;

        /// A pointer to the popup control for the popup menu.
        EditorPopupControl* m_pFileMenu;
    };
}

#endif
