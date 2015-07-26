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
        ///
        /// @return True if the tab was deleted; false if the tab could not be found (it could be part of a different tab bar).
        void DeleteTab(EditorTab* pTab);


        /// Retrieves the tab by it's GUI element.
        ///
        /// @param hElement [in] The GUI element of the tab control.
        EditorTab* GetTabByGUIElement(HGUIElement hElement) const;


        /// Retrieves a pointer to the currently active tab.
        EditorTab* GetActiveTab() const;

        /// Activates the given tab.
        ///
        /// @param pTab [in] A pointer to the tab to activate.
        ///
        /// @remarks
        ///     Passing null is equivalent to a call to DeactivateTab().
        void ActivateTab(EditorTab* pTab);

        /// Deactivates the currently active tab.
        void DeactivateActiveTab();

        /// Determines whether or not the tab is part of this group.
        bool IsTabOnThisBar(EditorTab* pTab) const;

        /// Retrieves a pointer ot the neighbouring tab, if any.
        ///
        /// @remarks
        ///     If there are not other tabs, this will return null.
        ///     @par
        ///     This will prioritize the tab to the right. If there are no tabs to the right, it will pick the one on the left.
        EditorTab* GetNeighbouringTab(EditorTab* pTab) const;


        /// Retrieves the number of tabs on the tab bar.
        size_t GetTabCount() const;

        /// Retrieves a pointer to the tab at the given index.
        EditorTab* GetTabByIndex(size_t index) const;


    private:

        class TabEventHandler : public GUIEventHandler
        {
        public:

            /// Constructor.
            TabEventHandler(EditorTabBar* pTabBar);

            void OnMouseEnter(GUIContext &gui, HGUIElement hElement);
            void OnMouseLeave(GUIContext &gui, HGUIElement hElement);
            void OnMouseButtonPressed(GUIContext &gui, HGUIElement hElement, int, int, int);



        private:

            /// A pointer to the tab bar control that owns the tab.
            EditorTabBar* m_pTabBar;
        };



    private:

        /// Updates the border mask for the active tab.
        void UpdateActiveTabBorderMask();


    private:

        /// The list of tabs. This is not in the order they appear in on the bar.
        GTLib::Vector<EditorTab*> m_tabs;

        /// A pointer to the active tab.
        EditorTab* m_pActiveTab;


        /// The parent element for tabs.
        HGUIElement m_hTabContainer;

        /// The parent element for the drop-down button.
        HGUIElement m_hDropDownContainer;


        /// The GUI event handler for each tab.
        TabEventHandler m_tabEventHandler;
    };
}

#endif // !__GT_EditorTabBat_hpp_
