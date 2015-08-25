// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorBodyControl
#define GT_EditorBodyControl

#include "EditorTabGroup.hpp"
#include "EditorAssetExplorer.hpp"

namespace GT
{
    /// Control representing the central part of the editor window.
    class EditorBody : public EditorControl
    {
    public:

        /// Constructor.
        EditorBody(Editor &editor);

        /// Destructor.
        ~EditorBody();


        HGUIElement GetLeftContainer() const { return m_hLeftContainer; }
        HGUIElement GetMiddleContainer() const { return m_hMiddleContainer; }
        HGUIElement GetRightContainer() const { return m_hRightContainer; }


        /// Retrieves the number of tab groups.
        size_t GetTabGroupCount() const;

        /// Retrieves the tab group at the given index.
        EditorTabGroup* GetTabGroupByIndex(size_t index) const;


        /// Closes the given tab.
        bool CloseTab(EditorTab* pTab);

        /// Activates the given tab.
        bool ActivateTab(EditorTab* pTab);

        /// Retrieves a pointer to the tab group the given tab is part of, if any.
        ///
        /// @remarks
        ///     If the tab is not part of any group, null will be returned.
        EditorTabGroup* FindTabGroup(EditorTab* pTab);


    private:

        /// The container for the left section.
        HGUIElement m_hLeftContainer;

        /// The container for the middle section.
        HGUIElement m_hMiddleContainer;

        /// The container for the right section.
        HGUIElement m_hRightContainer;


        /// The asset explorer.
        EditorAssetExplorer* m_pAssetExplorer;

        /// The list of tab groups.
        GTLib::Vector<EditorTabGroup*> m_tabGroups;
    };
}

#endif
