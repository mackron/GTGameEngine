// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorTabBar_hpp_
#define __GT_EditorTabBar_hpp_

#include "EditorTab.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    /// Control for listing a row of tabs.
    class EditorTabBar : public EditorControl
    {
    public:

        /// Constructor.
        EditorTabBar(Editor &editor);


        /// Creates and inserts a new tab.
        ///
        /// @param text [in] The text of the tab.
        EditorTab* CreateTab(const char* text);

        /// Removes the given tab from the tab bar and deletes it.
        ///
        /// @param pTab [in] A pointer to the tab to delete.
        void DeleteTab(EditorTab* pTab);



    private:

        /// The list of tabs. This is not in the order they appear in on the bar.
        GTLib::Vector<EditorTab*> m_tabs;


        /// The parent element for tabs.
        HGUIElement m_hTabContainer;

        /// The parent element for the drop-down button.
        HGUIElement m_hDropDownContainer;
    };
}

#endif // !__GT_EditorTabBat_hpp_
