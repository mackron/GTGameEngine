// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorScrollableControl_hpp_
#define __GT_EditorScrollableControl_hpp_

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
            DeadSpaceSizingEventHandler(HGUIElement hDeadSpace, HGUIElement hVerticalNeighbour, HGUIElement hHorizontalNeighbour)
                : m_hDeadSpace(hDeadSpace), m_hVerticalNeighbour(hVerticalNeighbour), m_hHorizontalNeighbour(hHorizontalNeighbour)
            {
            }


            void OnSize(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height)
            {
                if (hElement == m_hVerticalNeighbour)
                {
                    context.SetElementWidth(m_hDeadSpace, context.PixelsToPointsX(hElement, static_cast<int>(width)));
                }
                else if (hElement == m_hHorizontalNeighbour)
                {
                    context.SetElementHeight(m_hDeadSpace, context.PixelsToPointsY(hElement, static_cast<int>(height)));
                }
            }


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
