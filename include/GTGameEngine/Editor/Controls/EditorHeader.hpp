// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorHeaderControl_hpp_
#define __GT_EditorHeaderControl_hpp_

#include "EditorControl.hpp"
#include "EditorMainMenuBar.hpp"

namespace GT
{
    /// Control representing 
    class EditorHeader: public EditorControl
    {
    public:

        /// Constructor.
        EditorHeader(Editor &editor, HWindow hParentWindow);

        /// Destructor.
        ~EditorHeader();


    private:

        /// The main menu bar control.
        EditorMainMenuBar m_menuBar;
    };
}

#endif
