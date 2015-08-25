// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorGUISurfaceAUXData
#define GT_EditorGUISurfaceAUXData

#include "../WindowManager.hpp"

namespace GT
{
    /// Structure containing auxiliary data for surfaces.
    struct EditorGUISurfaceAUXData
    {
        EditorGUISurfaceAUXData(HWindow hWindowIn)
            : hWindow(hWindowIn),
              hMemDC(NULL), hMemBitmap(NULL), memBitmapWidth(0), memBitmapHeight(0)
        {
        }

        /// The window associated with the surface.
        HWindow hWindow;

        /// The off-screen DC for double-buffering.
        size_t hMemDC;

        /// The bitmap for double-buffering.
        size_t hMemBitmap;

        /// The width of the window at the time the offscreen DC and bitmap were created.
        int memBitmapWidth;

        /// The width of the window at the time the offscreen DC and bitmap were created.
        int memBitmapHeight;
    };
}

#endif
