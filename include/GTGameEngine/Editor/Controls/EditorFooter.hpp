// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorFooterControl
#define GT_EditorFooterControl

#include "EditorControl.hpp"

namespace GT
{
    /// Control representing
    class EditorFooter : public EditorControl
    {
    public:

        /// Constructor.
        EditorFooter(Editor &editor);

        /// Destructor.
        ~EditorFooter();


    private:

        /// The command text box.
        HGUIElement m_hCommandTextBox;
    };
}

#endif
