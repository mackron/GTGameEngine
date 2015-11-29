// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_WindowEvent_hpp_
#define __GTLib_WindowEvent_hpp_

#include "Windowing/Keys.hpp"
#include "Windowing/MouseButtons.hpp"
#include "Window.hpp"

namespace GTLib
{
#if 0
    // The built-in event IDs.
    static const uint32_t WindowEvent_None                   = 0x000000000;
    static const uint32_t WindowEvent_Close                  = 0x000000001;
    static const uint32_t WindowEvent_Paint                  = 0x000000002;
    static const uint32_t WindowEvent_Size                   = 0x000000003;
    static const uint32_t WindowEvent_Move                   = 0x000000004;
    static const uint32_t WindowEvent_MouseMove              = 0x000000005;
    static const uint32_t WindowEvent_MouseButtonDown        = 0x000000006;
    static const uint32_t WindowEvent_MouseButtonUp          = 0x000000007;
    static const uint32_t WindowEvent_MouseButtonDoubleClick = 0x000000008;
    static const uint32_t WindowEvent_MouseWheel             = 0x000000009;
    static const uint32_t WindowEvent_KeyPressed             = 0x00000000A;     // No repeating. Use KeyDown for repeating.
    static const uint32_t WindowEvent_KeyReleased            = 0x00000000B;
    static const uint32_t WindowEvent_KeyDown                = 0x00000000C;     // Repeating.
    static const uint32_t WindowEvent_KeyUp                  = 0x00000000D;
    static const uint32_t WindowEvent_ReceiveFocus           = 0x00000000E;     // When the window is brought to the foreground.
    static const uint32_t WindowEvent_LoseFocus              = 0x00000000F;     // When the window is taken away from the foreground.
    static const uint32_t WindowEvent_Timer                  = 0x000000010;     // When a timer is triggered.
    
    
    /// Structure containing the data for the WindowEvent_Close event.
    struct WindowEventData_Close
    {
    };
    
    /// Structure containing the data for the WindowEvent_Paint event.
    struct WindowEventData_Paint
    {
        /// The left side of the rectangle to paint.
        int left;
        
        /// The right side of the rectangle to paint.
        int right;
        
        /// The top side of the rectangle to paint.
        int top;
        
        /// The bottom side of the rectangle to paint.
        int bottom;
    };
    
    /// Structure containing the data for the WindowEvent_Size event.
    struct WindowEventData_Size
    {
        /// The new width of the window.
        unsigned int width;
        
        /// The new height of the window.
        unsigned int height;
    };
    
    /// Structure containing the data for the WindowEvent_Move event.
    struct WindowEventData_Move
    {
        /// The new position of the window on the x axis.
        int xPosition;
        
        /// The new position of the window on the y axis.
        int yPosition;
    };
    
    /// Structure containing the data for the WindowEvent_MouseMove event.
    struct WindowEventData_MouseMove
    {
        /// The new x position of the mouse cursor.
        int xPosition;
        
        /// The new y position of the mouse cursor.
        int yPosition;
    };
    
    /// Structure containing the data for the WindowEvent_MouseButtonDown event.
    struct WindowEventData_MouseButtonDown
    {
        /// The x position of the mouse cursor.
        int xPosition;
        
        /// The y position of the mouse cursor.
        int yPosition;
        
        /// The button that was pressed.
        unsigned int button;
    };
    
    /// Structure containing the data for the WindowEvent_MouseButtonUp event.
    struct WindowEventData_MouseButtonUp
    {
        /// The x position of the mouse cursor.
        int xPosition;
        
        /// The y position of the mouse cursor.
        int yPosition;
        
        /// The button that was released.
        unsigned int button;
    };
    
    /// Structure containing the data for the WindowEvent_MouseButtonDoubleClick event.
    struct WindowEventData_MouseButtonDoubleClick
    {
        /// The x position of the mouse cursor.
        int xPosition;
        
        /// The y position of the mouse cursor.
        int yPosition;
        
        /// The button that was double-clicked.
        unsigned int button;
    };
    
    /// Structure containing the data for the WindowEvent_MouseWheel event.
    struct WindowEventData_MouseWheel
    {
        /// The x position of the mouse cursor.
        int xPosition;
        
        /// The y position of the mouse cursor.
        int yPosition;
        
        /// The mouse wheel delta. This will usually be 1 or -1, but on windows can be multiples.
        int delta;
    };
    
    /// Structure containing the data for the WindowEvent_KeyPressed event.
    struct WindowEventData_KeyPressed
    {
        /// The key that was pressed.
        Key key;
    };
    
    /// Structure containing the data for the WindowEvent_KeyReleased event.
    struct WindowEventData_KeyReleased
    {
        /// The key that was released.
        Key key;
    };
    
    /// Structure containing the data for the WindowEvent_KeyDown event.
    struct WindowEventData_KeyDown
    {
        /// The key that was pressed.
        Key key;
    };
    
    /// Structure containing the data for the WindowEvent_KeyUp event.
    struct WindowEventData_KeyUp
    {
        /// The key that was released.
        Key key;
    };
    
    /// Structure containing the data for the WindowEvent_ReceiveFocus event.
    struct WindowEventData_ReceiveFocus
    {
    };
    
    /// Structure containing the data for the WindowEvent_LoseFocus event.
    struct WindowEventData_LoseFocus
    {
    };
    
    /// Structure containing the data for the WindowEvent_Timer event.
    struct WindowEventData_Timer
    {
        /// The ID of the timer that was triggered.
        uint32_t timerID;
    };
    
    
    
    /// Structure representing a window event.
    struct WindowEvent
    {
        /// The handle to the window this event is for.
        WindowHandle window;
        
        /// The event code.
        uint32_t code;
        
        /// The event data.
        const void* data;
    };
#endif
}

#endif // !__GTLib_WindowEvent_hpp_
