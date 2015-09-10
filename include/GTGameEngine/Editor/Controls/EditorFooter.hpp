// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorFooterControl
#define GT_EditorFooterControl

#include "EditorControl.hpp"

namespace GT
{
    /// Control representing the bar that runs along the bottom of the editor.
    class EditorFooter : public EditorControl
    {
    public:

        /// Constructor.
        EditorFooter(Editor &editor);

        /// Destructor.
        ~EditorFooter();


    private:

        // Currently, the footer is divided up into two sections - left and right. The left side contains the command text box and
        // the right side contains the other stuff.

        /// The container for the left side.
        HGUIElement m_hLeftContainer;

        /// The container for the right side.
        HGUIElement m_hRightContainer;


        /// The "Command" label. This may be temp and replaced with placeholder text in the command text box.
        HGUIElement m_hCommandLabel;

        /// The command text box.
        HGUIElement m_hCommandTextBox;
    };
}

#endif
