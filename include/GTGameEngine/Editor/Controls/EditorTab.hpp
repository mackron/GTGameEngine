// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorTab_hpp_
#define __GT_EditorTab_hpp_

#include "EditorControl.hpp"

namespace GT
{
    /// Class representing a single tab button on a tab bar.
    class EditorTab : public EditorControl
    {
    public:

        /// Constructor.
        EditorTab(Editor &editor);


        /// Sets the text of the tab.
        void SetText(const char* text);

        /// Retrieves the text of the tab.
        const char* GetText() const;


        /// Shows the close button.
        void ShowCloseButton();

        /// Hides the close button.
        void HideCloseButton();


    public:

        /// The element that will contain the text.
        HGUIElement m_hTextElement;

        /// The element that will contain the close button.
        HGUIElement m_hCloseElement;
    };
}

#endif // !__GT_EditorTab_hpp_
