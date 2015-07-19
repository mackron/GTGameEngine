// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorBodyControl_hpp_
#define __GT_EditorBodyControl_hpp_

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
