// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_ClippingVolume
#define GT_GUI_ClippingVolume

#include "Vertex.hpp"

namespace GTGUI
{
    /// Structure that will be used to clip elements against.
    ///
    /// The clipping volume always starts out in the shape of a rectangle.
    struct ClippingVolume
    {
        /// Constructor.
        ClippingVolume(float left, float top, float right, float bottom);

        /// Destructor.
        ~ClippingVolume();


        float left;
        float top;
        float right;
        float bottom;
    };
}


#endif