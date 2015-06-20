// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_Surface_hpp_
#define __GT_GUI_Surface_hpp_

#include <GTLib/String.hpp>
#include <GTLib/Vector.hpp>
#include "GUIElement.hpp"
#include "GUIRenderer.hpp"

namespace GT
{
    /// The data structures for surfaces.
    ///
    /// A surface is the thing that elements are displayed on. Any number of surfaces can be created, with any number
    /// of elements attached to it.
    struct GUISurface
    {
        /// Constructor.
        ///
        /// @param handle [in] The handle to the surface.
        GUISurface();

        /// Destructor.
        ~GUISurface();


        /// The ID of the surface. This is a string because we want it to be possible to assign elements to surfaces by a human-readable name.
        GTLib::String id;

        /// The width of the surface.
        unsigned int width;

        /// The height of the surface.
        unsigned int height;


        /// The current DPI on the X axis.
        unsigned int xDPI;

        /// The current DPI on the Y axis.
        unsigned int yDPI;


        /// The list of top-level elements that are on this surface. The elements in this list should all be parentless.
        GTLib::Vector<GUIElement*> topLevelElements;


        /// A pointer to the aux. data.
        void* auxData;


        /// The rectangle region containing the area that is invalid and needs to be re-painted. As a region is invalidated, this rectangle
        /// will be expanded to encompass it. Later on we may optimize this such that it's more efficiently broken up into seperate pieces
        /// based on the spacial location of each rectangle.
        GTLib::Rect<int> invalidRect;


    private:    // No copying.
        GUISurface(const GUISurface &);
        GUISurface & operator=(const GUISurface &);
    };


    struct GUISurfaceWithHandle : public GUISurface
    {
        GUISurfaceWithHandle(HGUISurface handle)
            : GUISurface(),
              handle(handle)
        {
        }

        /// The handle to the surface.
        HGUISurface handle;
    };
}

#endif