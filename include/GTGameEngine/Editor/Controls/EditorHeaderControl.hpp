// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorHeaderControl_hpp_
#define __GT_EditorHeaderControl_hpp_

#include "EditorControl.hpp"
#include "EditorMainMenuBarControl.hpp"

namespace GT
{
    /// Control representing 
    class EditorHeaderControl : public EditorControl
    {
    public:

        /// Constructor.
        EditorHeaderControl(GUIContext &gui);

        /// Destructor.
        ~EditorHeaderControl();


    private:

        /// The main menu bar control.
        EditorMainMenuBarControl m_menuBar;
    };
}

#endif
