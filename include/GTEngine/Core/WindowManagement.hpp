// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_WindowManagement_hpp_
#define __GTLib_WindowManagement_hpp_

#include "WindowEventCallback.hpp"

namespace GTLib
{
    /// Processes the next message in the event queue.
    ///
    /// @param callback [out] A reference to the WindowEventCallback that will handle the events.
    /// @param wait     [in]  Whether or not the function should wait for an event before returning.
    ///
    /// @return If some kind of error occurs, false will be returned.
    ///
    /// @remarks
    ///     This is NOT thread safe.
    ///     @par
    ///     This will remove the event from the queue.
    ///     @par
    ///     Multiple events may be processed for each call this this method. Conversely, it's possible nothing will be posted to the callback.
    ///     @par
    ///     If 'wait' is set to false and there are no messages in the queue, this will return immediately without anything being processed
    ///     by the callback.
    bool NextWindowEvent(WindowEventCallback &callback, bool wait = true);
    
    
    /// Sends an event to the given window.
    ///
    /// @param window    [in] A reference to the window to send the message to.
    /// @param eventCode [in] The event code.
    /// @param eventData [in] A pointer to the event data.
    ///
    /// @remarks
    ///     The event data is not copied. All this does is transmits the pointer exactly as is.
           void SendWindowEvent(Window* window, uint32_t eventCode, const void* eventData);
    inline void SendWindowEvent(Window &window, uint32_t eventCode, const void* eventData) { SendWindowEvent(&window, eventCode, eventData); }
    
    
    /// Initialises the window management module of GTLib.
    ///
    /// @remarks
    ///     Technically this is only needed for X11 setups, but it doesn't hurt to require it on Windows, too.
    ///     @par
    ///     This will create an X11 display which can be retrieved with GetX11Display(), which will only be defined
    ///     on the Linux build.
    ///     @par
    ///     This should be called once, before creating and windows.
    bool StartupWindowManager();
    
    /// Shuts down the window management module of GTLib.
    ///
    /// @remarks
    ///     On Linux builds, this will delete the X11 display that was created with StartupWindowManager().
    ///     @par
    ///     This should be called after all windows have been destroyed.
    void ShutdownWindowManager();
    
    
    /// Creates a timer associated with a window, which will trigger an OnTimer() event.
    ///
    /// @param window                [in] The window the timer should be associated with.
    /// @param timerID               [in] A non-zero ID for identifying the timer.
    /// @param timeoutInMilliseconds [in] The time in milliseconds to wait for the timer. See remarks.
    ///
    /// @remarks
    ///     The timeout (timeoutInMilliseconds) on Windows has a minimum allowable value of 0x0000000A and the maximium allowable
    ///     value of 0x7FFFFFFF. For consistency, this limiation is also placed on the Linux build.
    ///     @par
    ///     The window must be owned by the calling thread. This is, again, for consistency with Windows, but shouldn't be too big of
    ///     a problem in practice.
    ///     @par
    ///     The timer can be stopped with StopWindowTimer().
    ///     @par
    ///     The accuracy of the timer is not well defined. Don't user this for high resolution timing.
    ///     @par
    ///     On Linux, each timer runs in it's own sleeping thread. The sleep operation is performed with usleep(). When usleep() returns,
    ///     the event will be posted, but will be handled in the same thread as the call to NextWindowEvent().
    ///     @par
    ///     On Windows, the SetTimer/KillTimer API is used. Refer to the documentation of those functions for additional information
    ///     if needed. We do not use the 'lpTimerFunc' parameter.
    ///
    /// @see
    ///     GTLib::StopWindowTimer()
    void StartWindowTimer(WindowHandle window, uint32_t timerID, unsigned int timeoutInMilliseconds);
    
    /// Stops a timer that was created with StartWindowTimer().
    ///
    /// @param window  [in] The same window that was passed to StartWindowTimer().
    /// @param timerID [in] The ID of the timer. Again, this is the same timer that was passed to the corresponding call to SetWindowTimer().
    ///
    /// @remarks
    ///     There is a small window of opportunity where the event handler may be in the middle of processing when this function is
    ///     called. In this case, the event handler will still be processed. This is very unlikely to happen, though.
    ///
    /// @see
    ///     GTLib::StartWindowTimer()
    void StopWindowTimer(WindowHandle window, uint32_t timerID);
    
    
#if defined(__linux__)
    /// Retrieves the X11 display that was created in StartupWindowManager().
    ///
    /// @remarks
    ///     If the display has not yet been created, this will return null.
    ///     @par
    ///     This will return null if called after ShutdownWindowManager().
    Display* GetX11Display();
#endif
}

#endif
