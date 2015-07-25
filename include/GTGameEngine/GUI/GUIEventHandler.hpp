// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_GUI_EventHandler_hpp_
#define __GT_GUI_EventHandler_hpp_

#include <cstdint>
#include "GUIDataTypes.hpp"
#include <GTLib/Rect.hpp>
#include <GTLib/Vector.hpp>
#include <functional>

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
        /// @param x        [in] The new position of the element on the x axis, relative to the top left corner of the parent.
        /// @param y        [in] The new position of the element on the y axis, relative to the top left corner of the parent.
        virtual void OnMove(GUIContext &context, HGUIElement hElement, int x, int y);

        /// Called when an element's position has changed.
        ///
        /// @param context  [in] The GUI context.
        /// @param hElement [in] The element receiving the event.
        /// @param width    [in] The new width of the element.
        /// @param height   [in] The new height of the element.
        virtual void OnSize(GUIContext &context, HGUIElement hElement, unsigned int width, unsigned int height);


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

        /// Called when a mouse button is pressed while on top of the given element, or if a button is pressed while the given element has the mouse capture.
        ///
        /// @param context     [in] The GUI context.
        /// @param hElement    [in] The element receiving the event.
        /// @param mouseButton [in] The index of the mouse button that was pressed.
        /// @param mousePosX   [in] The position of the mouse relative to the top left corner, on the x axis.
        /// @param mousePosY   [in] The position of the mouse relative to the top left corner, on the y axis.
        virtual void OnMouseButtonPressed(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// Called when a mouse button is released while on top of the given element, or if a button is released while the given element has the mouse capture.
        ///
        /// @param context     [in] The GUI context.
        /// @param hElement    [in] The element receiving the event.
        /// @param mouseButton [in] The index of the mouse button that was released.
        /// @param mousePosX   [in] The position of the mouse relative to the top left corner, on the x axis.
        /// @param mousePosY   [in] The position of the mouse relative to the top left corner, on the y axis.
        virtual void OnMouseButtonReleased(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// Called when a mouse button is double-clicked while on top of the given element, or if a button is double-clicked while the given element has the mouse capture.
        ///
        /// @param context     [in] The GUI context.
        /// @param hElement    [in] The element receiving the event.
        /// @param mouseButton [in] The index of the mouse button that was double-clicked.
        /// @param mousePosX   [in] The position of the mouse relative to the top left corner, on the x axis.
        /// @param mousePosY   [in] The position of the mouse relative to the top left corner, on the y axis.
        virtual void OnMouseButtonDoubleClicked(GUIContext &context, HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY);

        /// Called when the mouse wheel is moved.
        ///
        /// @param context   [in] The GUI context.
        /// @param hElement  [in] The element receiving the event.
        /// @param delta     [in] The mouse wheel delta.
        /// @param mousePosX [in] The position of the mouse relative to the top left corner, on the x axis.
        /// @param mousePosY [in] The position of the mouse relative to the top left corner, on the y axis.
        virtual void OnMouseWheel(GUIContext &context, HGUIElement hElement, int delta, int mousePosX, int mousePosY);


        /// Called when an element receives the mouse event capture.
        ///
        /// @param context     [in] The GUI context.
        /// @param hElement    [in] The element receiving the event, and has just received the mouse event capture.
        virtual void OnSetMouseEventCapture(GUIContext &context, HGUIElement hElement);

        /// Called when an element is released of the mouse event capture.
        ///
        /// @param context     [in] The GUI context.
        /// @param hElement    [in] The element receiving the event, and has just received the mouse event capture.
        virtual void OnReleaseMouseEventCapture(GUIContext &context, HGUIElement hElement);


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


    typedef std::function<void (int xPos, int yPos)>                            LocalOnMoveProc;
    typedef std::function<void (unsigned int width, unsigned int height)>       LocalOnSizeProc;
    typedef std::function<void ()>                                              LocalOnMouseEnterProc;
    typedef std::function<void ()>                                              LocalOnMouseLeaveProc;
    typedef std::function<void (int mousePosX, int mousePosY)>                  LocalOnMouseMoveProc;
    typedef std::function<void (int mouseButton, int mousePosX, int mousePosY)> LocalOnMouseButtonPressedProc;
    typedef std::function<void (int mouseButton, int mousePosX, int mousePosY)> LocalOnMouseButtonReleasedProc;
    typedef std::function<void (int mouseButton, int mousePosX, int mousePosY)> LocalOnMouseButtonDoubleClickedProc;
    typedef std::function<void (int delta, int mousePosX, int mousePosY)>       LocalOnMouseWheelProc;
    typedef std::function<void ()>                                              LocalOnSetMouseEventCaptureProc;
    typedef std::function<void ()>                                              LocalOnReleaseMouseEventCaptureProc;

    struct LocalCallbackEventHandlers
    {
        GTLib::Vector<LocalOnMoveProc>                     OnMove;
        GTLib::Vector<LocalOnSizeProc>                     OnSize;
        GTLib::Vector<LocalOnMouseEnterProc>               OnMouseEnter;
        GTLib::Vector<LocalOnMouseLeaveProc>               OnMouseLeave;
        GTLib::Vector<LocalOnMouseMoveProc>                OnMouseMove;
        GTLib::Vector<LocalOnMouseButtonPressedProc>       OnMouseButtonPressed;
        GTLib::Vector<LocalOnMouseButtonReleasedProc>      OnMouseButtonReleased;
        GTLib::Vector<LocalOnMouseButtonDoubleClickedProc> OnMouseButtonDoubleClicked;
        GTLib::Vector<LocalOnMouseWheelProc>               OnMouseWheel;
        GTLib::Vector<LocalOnSetMouseEventCaptureProc>     OnSetMouseEventCapture;
        GTLib::Vector<LocalOnReleaseMouseEventCaptureProc> OnReleaseMouseEventCapture;
    };



    typedef std::function<void (HGUIElement hElement, int xPos, int yPos)>                            GlobalOnMoveProc;
    typedef std::function<void (HGUIElement hElement, unsigned int width, unsigned int height)>       GlobalOnSizeProc;
    typedef std::function<void (HGUIElement hElement)>                                                GlobalOnMouseEnterProc;
    typedef std::function<void (HGUIElement hElement)>                                                GlobalOnMouseLeaveProc;
    typedef std::function<void (HGUIElement hElement, int mousePosX, int mousePosY)>                  GlobalOnMouseMoveProc;
    typedef std::function<void (HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY)> GlobalOnMouseButtonPressedProc;
    typedef std::function<void (HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY)> GlobalOnMouseButtonReleasedProc;
    typedef std::function<void (HGUIElement hElement, int mouseButton, int mousePosX, int mousePosY)> GlobalOnMouseButtonDoubleClickedProc;
    typedef std::function<void (HGUIElement hElement, int delta, int mousePosX, int mousePosY)>       GlobalOnMouseWheelProc;
    typedef std::function<void (HGUIElement hElement)>                                                GlobalOnSetMouseEventCaptureProc;
    typedef std::function<void (HGUIElement hElement)>                                                GlobalOnReleaseMouseEventCaptureProc;
    typedef std::function<void (HGUISurface hSurface, const GTLib::Rect<int> &rect)>                  GlobalOnSurfaceNeedsRepaintProc;

    struct GlobalCallbackEventHandlers
    {
        GTLib::Vector<GlobalOnMoveProc>                     OnMove;
        GTLib::Vector<GlobalOnSizeProc>                     OnSize;
        GTLib::Vector<GlobalOnMouseEnterProc>               OnMouseEnter;
        GTLib::Vector<GlobalOnMouseLeaveProc>               OnMouseLeave;
        GTLib::Vector<GlobalOnMouseMoveProc>                OnMouseMove;
        GTLib::Vector<GlobalOnMouseButtonPressedProc>       OnMouseButtonPressed;
        GTLib::Vector<GlobalOnMouseButtonReleasedProc>      OnMouseButtonReleased;
        GTLib::Vector<GlobalOnMouseButtonDoubleClickedProc> OnMouseButtonDoubleClicked;
        GTLib::Vector<GlobalOnMouseWheelProc>               OnMouseWheel;
        GTLib::Vector<GlobalOnSetMouseEventCaptureProc>     OnSetMouseEventCapture;
        GTLib::Vector<GlobalOnReleaseMouseEventCaptureProc> OnReleaseMouseEventCapture;
        GTLib::Vector<GlobalOnSurfaceNeedsRepaintProc>      OnSurfaceNeedsRepaint;
    };
}

#endif // !__GT_GUI_EventHandler_hpp_
