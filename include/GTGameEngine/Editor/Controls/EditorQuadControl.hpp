// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorQuadControl_hpp_
#define __GT_EditorQuadControl_hpp_

#include "EditorControl.hpp"

namespace GT
{
    /// The asset explorer control.
    class EditorQuadControl : public EditorControl
    {
    public:

        EditorQuadControl(Editor &editor);
        

        /// Retrieves a handle to the top container.
        HGUIElement GetTop();

        /// Retrieves a handle to the bottom container.
        HGUIElement GetBottom();


        /// Retrieves a handle to the top/left quadrant element.
        HGUIElement GetTopLeftQuadrant() const;

        /// Retrieves a handle to the top/right quadrant element.
        HGUIElement GetTopRightQuadrant() const;

        /// Retrieves a handle to the bottom/left quadrant element.
        HGUIElement GetBottomLeftQuadrant() const;

        /// Retrieves a handle to the bottom/left quadrant element.
        HGUIElement GetBottomRightQuadrant() const;



    private:

        // This is actually made up of 6 child controls. 2 containers - 1 for the top row and the other for the bottom row - and then the
        // four quadrant controls.

        /// The top row container.
        HGUIElement m_hTopContainer;

        /// The bottom row container.
        HGUIElement m_hBottomContainer;


        /// The top/left quadrant.
        HGUIElement m_hTopLeft;

        /// The top/right quadrant.
        HGUIElement m_hTopRight;

        /// The bottom/left quadrant.
        HGUIElement m_hBottomLeft;

        /// The bottom/right quadrant.
        HGUIElement m_hBottomRight;
    };
}

#endif
