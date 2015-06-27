// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorMenuBarControl_hpp_
#define __GT_EditorMenuBarControl_hpp_

#include "EditorControl.hpp"
#include "../EditorEventHandler.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    class EditorMenuBarButton;
    class EditorMenuBar;

    /// Event handler for menu bar button GUI elements.
    class EditorMenuBarButtonGUIEventHandler : public GUIEventHandler
    {
    public:

        /// Constructor.
        EditorMenuBarButtonGUIEventHandler(EditorMenuBar* pMenuBar);


    private:

        void OnMouseEnter(GUIContext &gui, HGUIElement hElement);
        void OnMouseLeave(GUIContext &gui, HGUIElement hElement);
        void OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int button, int xPos, int yPos);
        void OnMouseButtonReleased(GUIContext &gui, HGUIElement hElement, int button, int xPos, int yPos);


    private:

        /// A pointer the menu bar that owns the button.
        EditorMenuBar* m_pMenuBar;
    };


    /// The editor event handler for detecting events that will cause popup windows to close.
    class EditorMenuBarEditorEventHandler : public EditorEventHandler
    {
    public:

        /// Constructor.
        EditorMenuBarEditorEventHandler(EditorMenuBar* pMenuBar);


        void OnWindowDeactivated(HWindow hWindow);
        void OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos);


    private:

        /// A pointer to the relevant menu bar.
        EditorMenuBar* m_pMenuBar;
    };



    /// Control representing a menu bar.
    class EditorMenuBar : public EditorControl
    {
    public:

        /// Constructor.
        EditorMenuBar(Editor &editor);

        /// Destructor.
        ~EditorMenuBar();


        /// Adds a button to the menu bar.
        ///
        /// @param buttonText [in] The button's text.
        EditorMenuBarButton* CreateAndInsertButton(const char* buttonText);

        /// Removes a button.
        ///
        /// @param pButton [in] A pointer to the button to remove.
        void DeleteButton(EditorMenuBarButton* pButton);


        /// Activates the given button.
        ///
        /// @param pButton [in] A pointer to the button to activate.
        ///
        /// @remarks
        ///     Passing null for pButton is equivalent to calling DeactivateActiveButton().
        ///     @par
        ///     This will deactivate the currently active button. If pButton is the currently active button, this will do nothing.
        void ActivateButton(EditorMenuBarButton* pButton);

        /// Deactivates the currently active button.
        void DeactivateActiveButton();

        /// Retrieves a pointer to the currently active button.
        ///
        /// @return A pointer to the currently active button, or null if not button is active.
        EditorMenuBarButton* GetActiveButton() const;



        //////////////////////////////////
        // Events

        /// Called after a button is activated.
        ///
        /// @param pButton [in] The button that was just activated.
        virtual void OnButtonActivated(EditorMenuBarButton* pButton);

        /// Called after a button is deactivated.
        ///
        /// @param pButton [in] The button that is being deactivated.
        virtual void OnButtonDeactivated(EditorMenuBarButton* pButton);



        //////////////////////////////////
        // Inbound Events

        /// Called when the given menu button has the mouse enter it.
        virtual void _OnButtonMouseEnter(HGUIElement hButtonElement);

        /// Called when the given menu button has the mouse leave it.
        virtual void _OnButtonMouseLeave(HGUIElement hButtonElement);

        /// Called when the given menu button is pressed.
        virtual void _OnButtonPressed(HGUIElement hButtonElement);

        /// Called when a mouse button is pressed on the given window.
        virtual void _OnMouseButtonPressedOnWindow(HWindow hWindow, int button, int xPos, int yPos);


    protected:

        /// Finds the button control by it's GUI element.
        EditorMenuBarButton* GetButtonFromGUIElement(HGUIElement hElement) const;


    private:

        /// The list of buttons currently attached ot the menu bar.
        GTLib::Vector<EditorMenuBarButton*> m_buttons;

        /// The event handler to attach to each button.
        EditorMenuBarButtonGUIEventHandler m_buttonEventHandler;

        /// The editor event handler for detecting events that will trigger popup windows to close.
        EditorMenuBarEditorEventHandler m_editorEventHandler;


        /// A pointer to the currently active button.
        EditorMenuBarButton* m_pActiveButton;
    };
}

#endif
