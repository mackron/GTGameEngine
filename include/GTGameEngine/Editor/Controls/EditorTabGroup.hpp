// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "EditorTabBar.hpp"
#include "EditorTabPage.hpp"

namespace GT
{
    /// Class representing a group of tabs.
    ///
    /// This is essentially a container for a tab bar and a group of tab pages.
    class EditorTabGroup : public EditorControl
    {
    public:

        /// Constructor.
        EditorTabGroup(Editor &editor);


    private:

        /// The tab bar.
        EditorTabBar m_tabBar;

        /// The list of tab pages. There is a tab page for each tab in the tab bar.
        GTLib::Map<EditorTab*, EditorTabPage*> m_tabPages;

        /// The GUI element that acts as the container for tab pages. All tab pages will be a child to 
        HGUIElement m_hTabPageContainer;
    };
}
