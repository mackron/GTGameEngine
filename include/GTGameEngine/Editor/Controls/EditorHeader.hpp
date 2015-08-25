// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorHeaderControl
#define GT_EditorHeaderControl

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
