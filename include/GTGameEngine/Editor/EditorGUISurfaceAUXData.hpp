// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorGUISurfaceAUXData_hpp_
#define __GT_EditorGUISurfaceAUXData_hpp_

#include "../WindowManager.hpp"

namespace GT
{
    /// Structure containing auxiliary data for surfaces.
    struct EditorGUISurfaceAUXData
    {
        EditorGUISurfaceAUXData(HWindow hWindowIn)
            : hWindow(hWindowIn)
        {
        }

        /// The window associated with the surface.
        HWindow hWindow;
    };
}

#endif
