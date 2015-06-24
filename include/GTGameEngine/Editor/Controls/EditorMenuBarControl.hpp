// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorMenuBarControl_hpp_
#define __GT_EditorMenuBarControl_hpp_

#include "EditorControl.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    class EditorMenuBarButton;


    /// Event handler for menu bar button GUI elements.
    class EditorMenuBarButtonGUIEventHandler : public GUIEventHandler
    {
        void OnMouseEnter(GUIContext &gui, HGUIElement hElement);
        void OnMouseLeave(GUIContext &gui, HGUIElement hElement);
        void OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int button, int xPos, int yPos);
        void OnMouseButtonReleased(GUIContext &gui, HGUIElement hElement, int button, int xPos, int yPos);
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



    private:

        /// The list of buttons currently attached ot the menu bar.
        GTLib::Vector<EditorMenuBarButton*> m_buttons;

        /// The event handler to attach to each button.
        EditorMenuBarButtonGUIEventHandler m_buttonEventHandler;
    };
}

#endif
