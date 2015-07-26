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


        /// Creates tab.
        EditorTab* CreateTab(const char* title);

        /// Closes the given tab.
        void CloseTab(EditorTab* pTab);

        /// Activates the given tab.
        void ActivateTab(EditorTab* pTab);

        /// Activates the neibouring tab.
        void ActivateNeighbouringTab(EditorTab* pTab);

        /// Deactivates the currently active tab.
        void DeactivateActiveTab();

        /// Retrieves a pointer to the active tab.
        EditorTab* GetActiveTab() const;
        
        /// Retrieves a pointer ot the neighbouring tab, if any.
        ///
        /// @remarks
        ///     If there are not other tabs, this will return null.
        ///     @par
        ///     This will prioritize the tab to the right. If there are no tabs to the right, it will pick the one on the left.
        EditorTab* GetNeighbouringTab(EditorTab* pTab) const;

        /// Determines whether or not the tab is in this group.
        bool IsTabInThisGroup(EditorTab* pTab) const;

        /// Retrieves a pointer to the tab page associated with the given tab.
        EditorTabPage* GetTabPage(EditorTab* pTab) const;


    private:

        /// The tab bar.
        EditorTabBar m_tabBar;

        /// The list of tab pages. There is a tab page for each tab in the tab bar.
        GTLib::Map<EditorTab*, EditorTabPage*> m_tabPages;

        /// The GUI element that acts as the container for tab pages. All tab pages will be a child to 
        HGUIElement m_hTabPageContainer;
    };
}
