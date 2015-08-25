// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorScrollableControl
#define GT_EditorScrollableControl

#include "EditorQuadControl.hpp"

namespace GT
{
    /// The asset explorer control.
    class EditorScrollableControl : public EditorQuadControl
    {
    public:

        EditorScrollableControl(Editor &editor);


        /// Retrieves the GUI element that acts as the container for the main content of the control.
        HGUIElement GetContentElement() const;


    private:


        struct DeadSpaceSizingEventHandler : public GUIEventHandler
        {
            /// Constructor.
            DeadSpaceSizingEventHandler(HGUIElement hDeadSpace, HGUIElement hVerticalNeighbour, HGUIElement hHorizontalNeighbour);

            /// @copydoc GUIEventHandler::OnSize()
            void OnSize(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height);


        private:

            HGUIElement m_hDeadSpace;
            HGUIElement m_hVerticalNeighbour;
            HGUIElement m_hHorizontalNeighbour;
        };


        /// The event handler that is attached to the neighbours of the dead space quadrant.
        DeadSpaceSizingEventHandler m_deadSpaceSizingHandler;
    };
}

#endif
