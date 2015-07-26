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


        /// Sets the style of the tab to the default style.
        void ApplyDefaultStyle();

        /// Sets the style of the tab to the hovered style.
        void ApplyHoveredStyle();

        /// Sets the style of the tab to the activated style.
        void ApplyActivatedStyle();


    public:

        /// The element that will contain the text.
        HGUIElement m_hTextElement;

        /// The element that will contain the close button.
        HGUIElement m_hCloseElement;


        /////////////////////////////////////////
        // Statics

        static const uint32_t Style_PaddingLeft   = 0;
        static const uint32_t Style_PaddingRight  = 0;
        static const uint32_t Style_PaddingTop    = 0;
        static const uint32_t Style_PaddingBottom = 0;

        static const uint32_t Style_BorderWidth   = 1;
    };
}

#endif // !__GT_EditorTab_hpp_
