// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUViewport_hpp_
#define __GT_GE_GPUViewport_hpp_

namespace GT
{
    /// Structure representing a viewport.
    struct GPUViewport
    {
        /// The position of the viewport on the X axis.
        float x;

        /// The position of the viewport on the Y axis.
        float y;

        /// The width of the viewport.
        float width;

        /// The height of the viewport.
        float height;

        /// The near depth range.
        float depthRangeNear;

        /// The far depth range.
        float depthRangeFar;
    };
}

#endif
