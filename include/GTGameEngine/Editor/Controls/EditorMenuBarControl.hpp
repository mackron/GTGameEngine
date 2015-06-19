// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorMenuBarControl_hpp_
#define __GT_EditorMenuBarControl_hpp_

#include "EditorControl.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    class EditorMenuBarButtonControl;


    /// Event handler for menu bar button GUI elements.
    class EditorMenuBarButtonControlGUIEventHandler : public GUIEventHandler
    {
        void OnMouseEnter(GUIContext &gui, HGUIElement hElement);
        void OnMouseLeave(GUIContext &gui, HGUIElement hElement);
    };


    /// Control representing a menu bar.
    class EditorMenuBarControl : public EditorControl
    {
    public:

        /// Constructor.
        EditorMenuBarControl(GUIContext &gui);

        /// Destructor.
        ~EditorMenuBarControl();


        /// Adds a button to the menu bar.
        ///
        /// @param buttonText [in] The button's text.
        EditorMenuBarButtonControl* CreateAndInsertButton(const char* buttonText);

        /// Removes a button.
        ///
        /// @param pButton [in] A pointer to the button to remove.
        void DeleteButton(EditorMenuBarButtonControl* pButton);



    private:

        /// The list of buttons currently attached ot the menu bar.
        GTLib::Vector<EditorMenuBarButtonControl*> m_buttons;

        /// The event handler to attach to each button.
        EditorMenuBarButtonControlGUIEventHandler m_buttonEventHandler;
    };
}

#endif
