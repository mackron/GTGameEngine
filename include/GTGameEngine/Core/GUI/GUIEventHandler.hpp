// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_EventHandler_hpp_
#define __GT_GUI_EventHandler_hpp_

#include <cstdint>
#include "GUIDataTypes.hpp"
#include <GTLib/Rect.hpp>

namespace GT
{
    class GUIContext;

    /// Base class for event handlers in the GUI system.
    class GUIEventHandler
    {
    public:

        /// Constructor.
        GUIEventHandler();

        /// Destructor.
        virtual ~GUIEventHandler();


        /// Called when an element's size has changed.
        ///
        /// @param context  [in] The GUI context.
        /// @param hElement [in] The element receiving the event.
        /// @param width    [in] The new width of the element.
        /// @param height   [in] The new height of the element.
        virtual void OnMove(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height);

        /// Called when an element's position has changed.
        ///
        /// @param context  [in] The GUI context.
        /// @param hElement [in] The element receiving the event.
        /// @param x        [in] The new position of the element on the x axis, relative to the top left corner of the parent.
        /// @param y        [in] The new position of the element on the y axis, relative to the top left corner of the parent.
        virtual void OnSize(GUIContext &context, HGUIElement hElement, int x, int y);


        /// Called when the mouse enters the area of the given GUI element.
        ///
        /// @param context  [in] The GUI context.
        /// @param hElement [in] The element receiving the event.
        virtual void OnMouseEnter(GUIContext &context, HGUIElement hElement);

        /// Called when the mouse leaves the area of the given GUI element.
        ///
        /// @param context  [in] The GUI context.
        /// @param hElement [in] The element receiving the event.
        virtual void OnMouseLeave(GUIContext &context, HGUIElement hElement);

        /// Called when the mouse is moved within the given GUI element, or if the element has captured the mouse.
        ///
        /// @param context   [in] The GUI context.
        /// @param hElement  [in] The element receiving the event.
        /// @param mousePosX [in] The position of the mouse relative to the top left corner.
        /// @param mousePosY [in] The position of the mouse relative to the top left corner.
        ///
        /// @remarks
        ///     If the given element has captured the mouse input, this event will be fired even when the mouse is not over the element.
        virtual void OnMouseMove(GUIContext &context, HGUIElement hElement, int mousePosX, int mousePosY);



        ///////////////////////////////////////////
        // Global Events
        //
        // The events below will only be called on event handlers that are in global scope.

        /// Called when the given surface needs to be redrawn.
        ///
        /// @param context [in] The GUI context.
        /// @param surface [in] The surface to redraw.
        /// @param rect    [in] The rectangle region needing to be redrawn.
        virtual void OnSurfaceNeedsRepaint(GUIContext &context, HGUISurface hSurface, const GTLib::Rect<int> &rect);
    };
}

#endif // !__GT_GUI_EventHandler_hpp_
