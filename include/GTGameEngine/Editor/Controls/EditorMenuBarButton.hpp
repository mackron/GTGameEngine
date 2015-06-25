// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorMenuBarButton_hpp_
#define __GT_EditorMenuBarButton_hpp_

#include "EditorControl.hpp"

namespace GT
{
    class EditorMenuBarButton : public EditorControl
    {
    public:

        /// Constructor.
        EditorMenuBarButton(Editor &editor, const char* text);

        /// Destructor.
        ~EditorMenuBarButton();


        /// Activates the button.
        void Activate();

        /// Deactivates the button.
        void Deactivate();

        /// Determines whether or not the button is activated.
        bool IsActivated() const;


    private:

        /// Shows the button's border.
        void ShowBorder();

        /// Hides the button's border.
        void HideBorder();


    private:

        /// Flags for boolean values.
        uint32_t m_flags;



        //////////////////////////////////////
        // Flags

        static const uint32_t Flag_IsActivated = (1 << 0);      //< Is the button activated.
    };
}

#endif
