// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/WindowManagement.hpp>

#ifdef __linux__
#include <GTEngine/Core/Windowing/X11/X11.hpp>
#include <GTEngine/Core/Windowing/X11/X11Keys.hpp>
#include <GTEngine/Core/Threading.hpp>
#include <GTEngine/Core/Map.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include <cstring>

namespace GTLib
{
    #define DOUBLE_CLICK_TIME   400         // <-- Temporary until we get a proper system in place. Will probably use dconf to determine this.
    
    
    static Display* g_X11Display = nullptr;
    
    static int g_X11DisplaySocket = -1;
    
    static int         g_X11AUXSocketRecv = -1;     // <-- The socket data is received from.
    static int         g_X11AUXSocketSend = -1;     // <-- The socket data is sent from.
    static sockaddr_un g_X11AUXSocketName;
    
    static int g_X11MaxRecvSocket = -1;
    
    static uint32_t g_X11AUXSocketRecvBuffer[256];  // <-- 1KB.

    
    
    Atom g_atom_WM_DELETE_WINDOW = 0;
    Atom g_atom_PRIMARY          = 0;
    Atom g_atom_CLIPBOARD        = 0;
    Atom g_atom_TARGETS          = 0;
    Atom g_atom_TEXT             = 0;
    Atom g_atom_GT_WINDOW    = 0;
    
    // Flags.
    extern const uint32_t DeleteColormap             = (1 << 0);
    extern const uint32_t IsFullScreen               = (1 << 1);
    extern const uint32_t IsNextKeyPressAutoRepeated = (1 << 2);

    
    // Event codes.
    static const uint32_t AUXDataType_Timer       = (1 << 0);
    static const uint32_t AUXDataType_CustomEvent = (1 << 1);
    
    
    struct TimerThreadKey
    {
        TimerThreadKey(WindowHandle windowIn, uint32_t timerIDIn)
            : window(windowIn), timerID(timerIDIn)
        {
        }
        
        
        bool operator<(const TimerThreadKey & other) const
        {
            if (this->window < other.window)
            {
                return true;
            }
            else if (this->window == other.window)
            {
                return this->timerID < other.timerID;
            }
            
            return false;
        }
        
        bool operator>(const TimerThreadKey & other) const
        {
            if (this->window > other.window)
            {
                return true;
            }
            else if (this->window == other.window)
            {
                return this->timerID > other.timerID;
            }
            
            return false;
        }
        
        bool operator==(const TimerThreadKey &other) const
        {
            return this->window == other.window && this->timerID == other.timerID;
        }
        
        
        /// The window.
        WindowHandle window;
        
        /// The timer ID.
        uint32_t timerID;
    };
    
    struct TimerThread;
    
    
    /// Keeps track of active timers. This is keyed by the timer ID.
    GTLib::Map<TimerThreadKey, TimerThread*> g_ActiveTimerThreads;
    
    /// Keeps track of inactive timers. This is keyed by the timer ID.
    GTLib::Map<TimerThreadKey, TimerThread*> g_InactiveTimerThreads;
    
    /// A mutex controlling sychronization with g_ActiveTimerThreadData and g_InactiveTimerThreadData.
    GTLib::Threading::Mutex g_TimerThreadsMutex;
    
    
    struct AUXSocketDataHeader
    {
        AUXSocketDataHeader()
            : window(nullptr), type(0)
        {
        }
        
        AUXSocketDataHeader(WindowHandle windowIn, uint32_t typeIn)
            : window(windowIn), type(typeIn)
        {
        }
        
        /// The window the socket data is associated with.
        WindowHandle window;
        
        /// The type of the data.
        uint32_t type;
    };
    
    struct AUXSocketTimerData
    {
        AUXSocketTimerData(WindowHandle window, uint32_t timerIDIn = 0)
            : header(window, AUXDataType_Timer), timerID(timerIDIn), flags(0)
        {
        }
        
        /// The base socket data. This must be set with valid data when sending.
        AUXSocketDataHeader header;
        
        /// The tiemr ID.
        uint32_t timerID;
        
        /// Flags detailing information about the timer.
        uint32_t flags;
        
        
        // Flags
        static const uint32_t IsStopped = (1 << 0);
    };
    
    
    /// The function to execute on the timer thread.
    void TimerThreadProc(void* data);
    
    /// A callback that will be passed to the timer thread.
    struct TimerThread
    {
        TimerThread(WindowHandle window, uint32_t timerID, unsigned int timeoutInMillisecondsIn)
            : thread(), timerData(window, timerID), timeoutInMilliseconds(timeoutInMillisecondsIn)
        {
        }
        
        ~TimerThread()
        {
        }
        
        
        /// Starts executing the thread.
        void Start()
        {
            this->thread.Start(TimerThreadProc, this, false);       // <-- 'false' means to not bother blocking.
        }
        
        
        /// Performs a sleep operation for the amount of time specified by timeoutInMilliseconds.
        void Wait()
        {
            Threading::Sleep(this->timeoutInMilliseconds);
        }
        
        /// Posts the event data through the g_X11AUXSocketSend socket.
        void PostEventData()
        {
            sendto(g_X11AUXSocketSend, &timerData, sizeof(timerData), 0, reinterpret_cast<sockaddr*>(&g_X11AUXSocketName), sizeof(g_X11AUXSocketName));
        }
        
        
        /// The thread itself.
        Threading::Thread thread;
                
        /// The timer data. This will be sent throught the AUX socket to be handled by the event handler.
        AUXSocketTimerData timerData;
        
        /// The timeout in milliseconds.
        unsigned int timeoutInMilliseconds;
    };
    
    void TimerThreadProc(void* data)
    {
        auto threadData = reinterpret_cast<TimerThread*>(data);
        assert(threadData != nullptr);
        {
            threadData->Wait();
            threadData->PostEventData();
        }
    }
    
    
    
    bool StartupWindowManager()
    {
        // Don't do anything if we're already initialized.
        if (g_X11Display == nullptr)
        {
            g_X11Display = XOpenDisplay(nullptr);
            if (g_X11Display != nullptr)
            {
                g_atom_WM_DELETE_WINDOW = XInternAtom(g_X11Display, "WM_DELETE_WINDOW", True);
                g_atom_PRIMARY          = XInternAtom(g_X11Display, "PRIMARY",          True);
                g_atom_CLIPBOARD        = XInternAtom(g_X11Display, "CLIPBOARD",        True);
                g_atom_TARGETS          = XInternAtom(g_X11Display, "TARGETS",          True);
                g_atom_TEXT             = XInternAtom(g_X11Display, "TEXT",             True);
                g_atom_GT_WINDOW    = XInternAtom(g_X11Display, "GT_WINDOW",    False);     // <-- For assigning a GTLib::WindowHandle to an X11 window.
            }
        }
        
        if (g_X11Display != nullptr)
        {
            g_X11DisplaySocket = ConnectionNumber(g_X11Display);
            
            g_X11AUXSocketRecv = socket(AF_LOCAL, SOCK_DGRAM, 0);
            if (g_X11AUXSocketRecv >= 0)
            {
                memset(&g_X11AUXSocketName, 0, sizeof(g_X11AUXSocketName));
                
                g_X11AUXSocketName.sun_family = AF_LOCAL;
                strcpy(g_X11AUXSocketName.sun_path + 1, "__gt_x11_aux_socket");
                
                if (bind(g_X11AUXSocketRecv, reinterpret_cast<sockaddr*>(&g_X11AUXSocketName), sizeof(g_X11AUXSocketName)) >= 0)
                {
                    g_X11AUXSocketSend = socket(AF_LOCAL, SOCK_DGRAM, 0);
                    if (g_X11AUXSocketSend >= 0)
                    {
#if 0
                        // Testing.
                        AUXSocketTimerData timerData(nullptr, 123);
                        sendto(g_X11AUXSocketSend, &timerData, sizeof(timerData), 0, reinterpret_cast<sockaddr*>(&g_X11AUXSocketName), sizeof(g_X11AUXSocketName));
#endif
                    }
                    else
                    {
                        printf("Error creating sending auxiliary socket: %s\n", strerror(errno));
                    
                        close(g_X11AUXSocketRecv);
                        g_X11AUXSocketRecv = -1;
                    }
                }
                else
                {
                    printf("Error binding receiving auxiliary socket: %s\n", strerror(errno));
                    
                    close(g_X11AUXSocketRecv);
                    g_X11AUXSocketRecv = -1;
                }
            }
            else
            {
                printf("Error creating receiving auxiliary socket: %s\n", strerror(errno));
            }
        }
        
        
        // We need the highest socket number for use with select().
        g_X11MaxRecvSocket = GTLib::Max(g_X11DisplaySocket, g_X11AUXSocketRecv);
        
        
        return g_X11Display != nullptr;
    }
    
    void ShutdownWindowManager()
    {
        if (g_X11Display != nullptr)
        {
            XCloseDisplay(g_X11Display);
            g_X11Display = nullptr;
        }
        
        close(g_X11AUXSocketRecv);
        close(g_X11AUXSocketSend);
        
        g_X11AUXSocketRecv = -1;
        g_X11AUXSocketSend = -1;
        
        g_X11DisplaySocket = -1;
        
        g_X11MaxRecvSocket = -1;
    }
    
    

    void StopWindowTimer_NoLock(WindowHandle window, uint32_t timerID)
    {
        auto threadKey = TimerThreadKey(window, timerID);
        
        auto iActiveTimer = g_ActiveTimerThreads.Find(threadKey);
        if (iActiveTimer != nullptr)
        {
            assert(g_InactiveTimerThreads.Exists(threadKey) == false);
            
            auto timerThreadData = iActiveTimer->value;
            assert(timerThreadData != nullptr);
            {
                timerThreadData->timerData.flags |= AUXSocketTimerData::IsStopped;
                
                g_InactiveTimerThreads.Add(threadKey, timerThreadData);
                g_ActiveTimerThreads.Remove(iActiveTimer->index);
            }
        }
    }
    
    
    TimerThread* CreateTimerThread_NoLock(WindowHandle window, uint32_t timerID, unsigned int timeoutInMilliseconds)
    {
        auto timerThread = new TimerThread(window, timerID, timeoutInMilliseconds);
        g_ActiveTimerThreads.Add(TimerThreadKey(window, timerID), timerThread);
        
        return timerThread;
    }
    
    void DeleteTimerThread_NoLock(WindowHandle window, uint32_t timerID)
    {
        auto iInactiveThread = g_InactiveTimerThreads.Find(TimerThreadKey(window, timerID));
        assert(iInactiveThread != nullptr);
        {
            delete iInactiveThread->value;
            g_InactiveTimerThreads.RemoveByIndex(iInactiveThread->index);
        }
    }
    
    
    TimerThread* GetActiveTimerThread_NoLock(WindowHandle window, uint32_t timerID)
    {
        auto iActiveThread = g_ActiveTimerThreads.Find(TimerThreadKey(window, timerID));
        if (iActiveThread != nullptr)
        {
            return iActiveThread->value;
        }
        
        return nullptr;
    }
    
    
    void StartWindowTimer(WindowHandle window, uint32_t timerID, unsigned int timeoutInMilliseconds)
    {
        // For consistency with Windows, the timeout can not be less than 0x0000000A or more than 0x7FFFFFFF.
        if (timeoutInMilliseconds < 0x0000000A)
        {
            timeoutInMilliseconds = 0x0000000A;
        }
        if (timeoutInMilliseconds > 0x7FFFFFFF)
        {
            timeoutInMilliseconds = 0x7FFFFFFF;
        }
        
        
        TimerThread* timerThread = nullptr;
        
        g_TimerThreadsMutex.Lock();
        {
            // We need to check if a timer of the same ID is already active. If so the old timer thread needs to be stopped and
            // replaced with a new one.
            StopWindowTimer_NoLock(window, timerID);
            
            // Now we just create the thread.
            timerThread = CreateTimerThread_NoLock(window, timerID, timeoutInMilliseconds);
        }
        g_TimerThreadsMutex.Unlock();
        
        
        // Now start the thread. We want this to be outside of the above mutex lock.
        timerThread->Start();
    }
    
    void StopWindowTimer(WindowHandle window, uint32_t timerID)
    {
        // NOTE: Do not delete the timer thread from here! Instead, just MARK it for deletion. It will be deleted when the event handler receives an
        //       event from it.
        
        g_TimerThreadsMutex.Lock();
        {
            StopWindowTimer_NoLock(window, timerID);
        }
        g_TimerThreadsMutex.Unlock();
    }
    
    
    Display* GetX11Display()
    {
        return g_X11Display;
    }

    
    /// Converts an X11 mouse button index to the GTLib equivalent.
    unsigned int MapX11MouseButton(unsigned int x11MouseButton)
    {
        if (x11MouseButton == Button2)      // <-- Middle mouse button.
        {
            return 3;
        }
        if (x11MouseButton == Button3)      // <-- Right mouse button.
        {
            return 2;
        }
        
        if (x11MouseButton > Button5)       // <-- Button4 and Button5 represent the mouse wheel.
        {
            return x11MouseButton - 2;
        }
        
        // If we get here, the mouse button will be equal to Button1, Button4 or Button5. Button1 is the left mouse button, so
        // that's valid, but 4 and 5 are the mouse wheels. We want to return 0 if they were passed as input because GTLib does
        // not consider them buttons.
        if (x11MouseButton != Button1)
        {
            assert(x11MouseButton == Button4 || x11MouseButton == Button5);
            return 0;
        }
        
        
        // If we get here it means the X11 button code is the same as GTLib's.
        return x11MouseButton;
    }
    
    
    /// Handles the given X11 event, calling the appropriate methods in the given callback.
    ///
    /// @remarks
    ///     If this returns false, the event handler will simply skip the event.
    ///     @par
    ///     This can call multiple methods on the callback for a single call.
    bool HandleX11Event(XEvent &eventIn, WindowEventCallback &callback, WindowHandle window)
    {
        assert(window != nullptr);     // <-- The window should have been set before calling this function.
        
        switch (eventIn.type)
        {
        case ClientMessage:
            {
                if (static_cast<Atom>(eventIn.xclient.data.l[0]) == g_atom_WM_DELETE_WINDOW)
                {
                    callback.OnClose(window);
                    
                    return true;
                }

                break;
            }
            
        case Expose:
            {
                int left   = eventIn.xexpose.x;
                int top    = eventIn.xexpose.y;
                int right  = eventIn.xexpose.x + eventIn.xexpose.width;
                int bottom = eventIn.xexpose.y + eventIn.xexpose.height;
                callback.OnPaint(window, left, top, right, bottom, nullptr);        // <-- Last argument is only used for Win32.
                
                return true;
            }
            
        case ConfigureNotify:
            {
                const int currentPosX   = eventIn.xconfigure.x;
                const int currentPosY   = eventIn.xconfigure.y;
                const int prevPosX      = window->prevPosX;
                const int prevPosY      = window->prevPosY;
                
                const int currentWidth  = eventIn.xconfigure.width;
                const int currentHeight = eventIn.xconfigure.height;
                const int prevWidth     = window->prevWidth;
                const int prevHeight    = window->prevHeight;
                
                const bool positionChanged = currentPosX  != prevPosX  || currentPosY   != prevPosY;
                const bool sizeChanged     = currentWidth != prevWidth || currentHeight != prevHeight;
                
                if (positionChanged)
                {
                    window->prevPosX = currentPosX;
                    window->prevPosY = currentPosY;
                    
                    callback.OnMove(window, currentPosX, currentPosY);
                }

                if (sizeChanged)
                {
                    window->prevWidth  = currentWidth;
                    window->prevHeight = currentHeight;
                    
                    callback.OnSize(window, currentWidth, currentHeight);
                }
                
                return true;
            }
            
        case MotionNotify:
            {
                callback.OnMouseMove(window, eventIn.xmotion.x, eventIn.xmotion.y);
                
                return true;
            }
            
        case KeyPress:
            {
                Key key = X11::GetKeyFromX11KeyEvent(eventIn);
                            
                if ((window->flags & IsNextKeyPressAutoRepeated) == 0)
                {
                    // The KeyPressed event needs to be upper case.
                    Key upperCaseKey = key;
                    if (upperCaseKey >= 'a' && upperCaseKey <= 'z')
                    {
                        upperCaseKey -= 0x20;
                    }
                    
                    callback.OnKeyPressed(window, upperCaseKey);
                }
                
                // If we get here we just post a KeyDown event like normal. It's important that we unset IsNextKeyPressAutoRepeated flag here.
                callback.OnKeyDown(window, key);

                window->flags &= ~IsNextKeyPressAutoRepeated;
                return true;
            }
            
        case KeyRelease:
            {
                // We might be auto-repeating. In this case, we don't never want to post a KeyReleased or KeyUp event. We can detect this by peeking
                // at the next message. If it is a KeyPress event of the same key, it's an auto-repeated.
                XEvent nextEvent;
                if (XEventsQueued(eventIn.xany.display, QueuedAfterReading))
                {
                    XPeekEvent(eventIn.xany.display, &nextEvent);
                }
                else
                {
                    nextEvent.type = None;
                }
                
                if (nextEvent.type         == KeyPress    &&
                    nextEvent.xkey.time    == eventIn.xkey.time &&
                    nextEvent.xkey.keycode == eventIn.xkey.keycode)
                {
                    // The key is auto-repeated.
                    window->flags |= IsNextKeyPressAutoRepeated;
                }
                else
                {
                    // The key is not auto-repeated. We handle this just like normal.
                    window->flags &= ~IsNextKeyPressAutoRepeated;
                    
                    
                    // We now need to post the appropriate event. We need to post both KeyRelease and KeyUp. To do this, we first do KeyRelease,
                    // put the event back exactly as-is, then set a flag to have the next event handler iteration skip KeyRelease and do KeyUp.
                    Key key = X11::GetKeyFromX11KeyEvent(eventIn);
                    
                    Key upperCaseKey = key;
                    if (upperCaseKey >= 'a' && upperCaseKey <= 'z')
                    {
                        upperCaseKey -= 0x20;
                    }
                    
                    callback.OnKeyReleased(window, upperCaseKey);
                    callback.OnKeyUp(window, key);
                    
                    return true;
                }
                
                break;
            }
            
        case ButtonPress:
            {
                unsigned int button = 0;
                
                // Button4 and Button5 represent the mouse wheel.
                if (eventIn.xbutton.button == Button4 || eventIn.xbutton.button == Button5)
                {
                    int delta     = (eventIn.xbutton.button == Button4) ? 1 : -1;
                    int xPosition = eventIn.xbutton.x;
                    int yPosition = eventIn.xbutton.y;
                    
                    callback.OnMouseWheel(window, delta, xPosition, yPosition);
                }
                else
                {
                    button = MapX11MouseButton(eventIn.xbutton.button);
                    assert(button != 0);
                    {
                        int xPosition = eventIn.xbutton.x;
                        int yPosition = eventIn.xbutton.y;
                        
                        
                        // In X11, we need to manually check for double-clicks. To do this, we keep track of the last time a button
                        // is clicked.
                        //
                        // First thing we need to do is make sure we actually have an associated slot in our click-times array.
                        if (window->lastMouseButtonClickTimes.count <= button)
                        {
                            size_t prevCount = window->lastMouseButtonClickTimes.count;
                            window->lastMouseButtonClickTimes.Resize(button + 1);
                            
                            // Initialise new items to 0.
                            for (unsigned int iItem = prevCount; iItem < window->lastMouseButtonClickTimes.count; ++iItem)
                            {
                                window->lastMouseButtonClickTimes[iItem] = 0;
                            }
                        }
                        
                        Time thisTime = eventIn.xbutton.time;
                        Time lastTime = window->lastMouseButtonClickTimes[button];
                        
                        
                        // OnMouseButtonDown() is always sent, even when a double-click event is about to be posted.
                        callback.OnMouseButtonDown(window, button, xPosition, yPosition);
                        
                        
                        // This event could be intended to be handled as a double-click event. We can determing this by checking if 'lastTime' is equal
                        // to -1. If not, it should be handled just like any other click event. If so, it should be handled as a double-click event.
                        if (thisTime - lastTime <= DOUBLE_CLICK_TIME && lastTime != static_cast<Time>(-1))
                        {
                            // We have received a double-click event. After we do this, 
                            callback.OnMouseButtonDoubleClick(window, button, xPosition, yPosition);
                            
                            // We set the last click time to -1 so that we don't get another double-click event for the third click.
                            window->lastMouseButtonClickTimes[button] = static_cast<Time>(-1);
                        }
                        else
                        {
                            window->lastMouseButtonClickTimes[button] = eventIn.xbutton.time;
                        }
                    }
                }
                
                return true;
            }
            
        case ButtonRelease:
            {
                unsigned int button = 0;
                
                // Button4 and Button5 represent the mouse wheel.
                if (!(eventIn.xbutton.button == Button4 || eventIn.xbutton.button == Button5))
                {
                    button = MapX11MouseButton(eventIn.xbutton.button);
                    assert(button != 0);
                    {
                        callback.OnMouseButtonUp(window, button, eventIn.xbutton.x, eventIn.xbutton.y);
                    }
                }
                
                return true;
            }
            
        case FocusIn:
            {
                callback.OnReceiveFocus(window);
                
                return true;
            }
            
        case FocusOut:
            {
                callback.OnLoseFocus(window);
                
                return true;
            }
            
        default: break;
        }
        
        
        // If we get here it means we're not aware of the event type.
        return false;
    }
    
    // Helper function for retrieving the GTLib::WindowHandle from the X11 window.
    WindowHandle GetWindowHandleFromX11Window(Display* display, ::Window window)
    {
        WindowHandle result = nullptr;
        
        Atom           type;
        int            format;       // 8, 16, or 32.
        unsigned long  nitems;
        unsigned long  dataSizeInBytes;
        unsigned char* windowHandleBytes;
        if (XGetWindowProperty(display, window, g_atom_GT_WINDOW, 0, sizeof(WindowHandle), False, AnyPropertyType, &type, &format, &nitems, &dataSizeInBytes, &windowHandleBytes) == Success && windowHandleBytes != nullptr)
        {
            if (nitems == sizeof(WindowHandle))
            {
                result = *reinterpret_cast<WindowHandle*>(windowHandleBytes);
            }
            
            XFree(windowHandleBytes);
        }
        
        return result;
    }
    
    static XEvent g_X11Event;
    bool NextWindowEvent(WindowEventCallback &callback, bool wait)
    {
        auto display = GetX11Display();
        if (display != nullptr)
        {
            struct timeval tv;
            memset(&tv, 0, sizeof(tv));
            
            fd_set sockets;
            FD_ZERO(&sockets);
            FD_SET(g_X11DisplaySocket, &sockets);
            FD_SET(g_X11AUXSocketRecv, &sockets);
            
            // We need to wait using the socket API. This is because timer events will be posted to it outside of the normal X11 event system.
            if (select(g_X11MaxRecvSocket + 1, &sockets, nullptr, nullptr, (wait) ? nullptr : &tv))
            {
                // Normal X11 event.
                if (FD_ISSET(g_X11DisplaySocket, &sockets))
                {
                    while (XPending(display) > 0)
                    {
                        XNextEvent(display, &g_X11Event);
                        
                        auto window = GetWindowHandleFromX11Window(g_X11Event.xany.display, g_X11Event.xany.window);
                        if (window != nullptr)
                        {
                            HandleX11Event(g_X11Event, callback, window);
                        }
                    }
                }
                
                // Custom event.
                if (FD_ISSET(g_X11AUXSocketRecv, &sockets))
                {
                    ssize_t recvResult = recv(g_X11AUXSocketRecv, g_X11AUXSocketRecvBuffer, sizeof(g_X11AUXSocketRecvBuffer), 0);
                    if (recvResult > 0)
                    {
                        // We need data sizes. Everything is in 4-byte chunks from here on.
                        const size_t headerSize  = sizeof(AUXSocketDataHeader) / 4;
                        const size_t messageSize = recvResult / 4;
                        
                        size_t readPointer = 0;
                        while (readPointer + headerSize <= messageSize)
                        {
                            AUXSocketDataHeader header;
                            memcpy(&header, g_X11AUXSocketRecvBuffer + readPointer, sizeof(header));
                            
                            readPointer += headerSize;
                            
                            
                            switch (header.type)
                            {
                            case AUXDataType_Timer:
                                {
                                    // The next 4 bytes should be the timer ID.
                                    if (readPointer < messageSize)
                                    {
                                        uint32_t timerID = g_X11AUXSocketRecvBuffer[readPointer++];
                                        if (timerID > 0)        // <-- For consistency with Windows, we don't allow a timer ID of zero.
                                        {
                                            uint32_t timerFlags = g_X11AUXSocketRecvBuffer[readPointer++];
                                            
                                            if ((timerFlags & AUXSocketTimerData::IsStopped) != 0)
                                            {
                                                // The timer has stopped. Instead of posting OnTimer(), we want to delete it.
                                                g_TimerThreadsMutex.Lock();
                                                {
                                                    DeleteTimerThread_NoLock(header.window, timerID);
                                                }
                                                g_TimerThreadsMutex.Unlock();
                                            }
                                            else
                                            {
                                                // The timer is still active. Post the event.
                                                callback.OnTimer(header.window, timerID);
                                                
                                                // If the timer is still active, we want to restart it. If it's not active, we need to delete it.
                                                TimerThread* timerThread = nullptr;
                                                
                                                g_TimerThreadsMutex.Lock();
                                                {
                                                    timerThread = GetActiveTimerThread_NoLock(header.window, timerID);
                                                    if (timerThread == nullptr)
                                                    {
                                                        // If we get here, the thread is not active. That means it must have been stopped.
                                                        DeleteTimerThread_NoLock(header.window, timerID);
                                                    }
                                                }
                                                g_TimerThreadsMutex.Unlock();
                                                
                                                if (timerThread != nullptr)
                                                {
                                                    timerThread->Start();
                                                }
                                            }
                                        }
                                    }
                                    
                                    break;
                                }
                                
                            case AUXDataType_CustomEvent:
                                {
                                    // TODO: Implement.
                                    break;
                                }
                                
                            default:
                                {
                                    // We don't know what the data type is. We're going to flush the data and break.
                                    readPointer = messageSize;      // <-- break from the data read loop.
                                    break;                          // <-- break from the switch.
                                }
                            }
                        }
                        
                        //printf("Message Size: %d\n", static_cast<int>(recvResult));
                    }
                    else
                    {
                        if (recvResult == -1)
                        {
                            printf("Error receiving data from AUX socket: %s\n", strerror(errno));
                        }
                    }
                }
            }
            
            return true;
        }

        return false;
    }
    
    void SendWindowEvent(Window* window, uint32_t eventCode, const void* eventData)
    {
        (void)window;
        (void)eventCode;
        (void)eventData;
    }
}
#else
#include <GTEngine/Core/Windowing/Win32/Win32.hpp>
#include <GTEngine/Core/windows.hpp>
#include <windowsx.h>
#include <cstdio>

namespace GTLib
{
    const char* WindowClassName = "GT_DefaultWindow";

    /// The list of HCURSOR objects, each of which correspond to a SystemCursor.
    static HCURSOR Win32SystemCursors[Cursor_Count] = {0};

    // Flags.
    extern const uint32_t WindowCursorHidden = (1 << 0);
    extern const uint32_t CTRLKeyDown        = (1 << 1);


    unsigned int ConvertWin32MouseButton(UINT msg)
    {
        switch (msg)
        {
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            {
                return MouseButton_Left;
            }

        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            {
                return MouseButton_Right;
            }

        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            {
                return MouseButton_Middle;
            }

        default: break;
        }

        return MouseButton_Unknown;
    }

    Key GetKeyFromWin32VirtualKey(WPARAM key)
    {
        // Easy cases for letters and numbers...
        if (key >= 'A' && key <= 'Z')
        {
            return (Key)key;
        }
        if (key >= '0' && key <= '9')
        {
            return (Key)key;
        }

        switch (key)
        {
        case VK_PAUSE:      return Keys::Pause;
        case VK_SCROLL:     return Keys::ScrollLock;

        case VK_BACK:       return Keys::Backspace;
        case VK_TAB:        return Keys::Tab;
        case VK_DELETE:     return Keys::Delete;
        case VK_RETURN:     return Keys::Return;
        case VK_SPACE:      return Keys::Space;
        case VK_ESCAPE:     return Keys::Escape;

        case VK_SHIFT:      return Keys::Shift;
        case VK_CONTROL:    return Keys::Ctrl;
        case VK_MENU:       return Keys::Alt;
        case VK_LWIN:       return Keys::Super;
        case VK_RWIN:       return Keys::Super;
        case VK_CAPITAL:    return Keys::CapsLock;

#if 0
        case VK_LSHIFT:     return Keys::LeftShift;
        case VK_RSHIFT:     return Keys::RightShift;
        case VK_LCONTROL:   return Keys::LeftCtrl;
        case VK_RCONTROL:   return Keys::RightCtrl;
        case VK_LMENU:      return Keys::LeftAlt;
        case VK_RMENU:      return Keys::RightAlt;
#endif

        case VK_LEFT:       return Keys::ArrowLeft;
        case VK_RIGHT:      return Keys::ArrowRight;
        case VK_UP:         return Keys::ArrowUp;
        case VK_DOWN:       return Keys::ArrowDown;

        case VK_F1:         return Keys::F1;
        case VK_F2:         return Keys::F2;
        case VK_F3:         return Keys::F3;
        case VK_F4:         return Keys::F4;
        case VK_F5:         return Keys::F5;
        case VK_F6:         return Keys::F6;
        case VK_F7:         return Keys::F7;
        case VK_F8:         return Keys::F8;
        case VK_F9:         return Keys::F9;
        case VK_F10:        return Keys::F10;
        case VK_F11:        return Keys::F11;
        case VK_F12:        return Keys::F12;
        case VK_F13:        return Keys::F13;
        case VK_F14:        return Keys::F14;
        case VK_F15:        return Keys::F15;
        case VK_F16:        return Keys::F16;
        case VK_F17:        return Keys::F17;
        case VK_F18:        return Keys::F18;
        case VK_F19:        return Keys::F19;
        case VK_F20:        return Keys::F20;
        case VK_F21:        return Keys::F21;
        case VK_F22:        return Keys::F22;
        case VK_F23:        return Keys::F23;
        case VK_F24:        return Keys::F24;


        case VK_END:        return Keys::End;
        case VK_HOME:       return Keys::Home;
        case VK_PRIOR:      return Keys::PageUp;
        case VK_NEXT:       return Keys::PageDown;


        case VK_SELECT:     return Keys::Select;
        case VK_PRINT:      return Keys::Print;
        case VK_EXECUTE:    return Keys::Execute;
        case VK_INSERT:     return Keys::Insert;
        // Keys::Undo
        // Keys::Redo
        // Keys::Menu
        // Keys::Find
        // Keys::Cancel
        case VK_HELP:       return Keys::Help;
        case VK_CANCEL:     return Keys::Break;
        // Keys::ModeSwitch
        case VK_NUMLOCK:    return Keys::NumLock;



        default: break;
        }

        return '\0';
    }


    HCURSOR GetHCURSOR(Cursor cursor)
    {
        return Win32SystemCursors[cursor];
    }


    WindowEventCallback* g_NextCallback = nullptr;
    LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (g_NextCallback != nullptr)
        {
            auto window = reinterpret_cast<_Window*>(GetPropA(hWnd, "__gtc_window"));
            if (window != nullptr)
            {
                switch (msg)
                {
                case WM_CLOSE:
                    {
                        g_NextCallback->OnClose(window);

                        return 0;
                    }

                case WM_PAINT:
                    {
                        PAINTSTRUCT ps;
                        BeginPaint(hWnd, &ps);
                        {
                            g_NextCallback->OnPaint(window, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom, &ps);
                        }
                        EndPaint(hWnd, &ps);

                        return 0;
                    }

                case WM_SIZE:
                    {
                        g_NextCallback->OnSize(window, LOWORD(lParam), HIWORD(lParam));

                        return 0;
                    }

                case WM_MOVE:
                    {
                        g_NextCallback->OnMove(window, LOWORD(lParam), HIWORD(lParam));

                        return 0;
                    }


                case WM_NCMOUSEMOVE:
                    {
                        POINT temp;
                        temp.x = GET_X_LPARAM(lParam);
                        temp.y = GET_Y_LPARAM(lParam);
                        ScreenToClient(hWnd, &temp);

                        g_NextCallback->OnMouseMove(window, temp.x, temp.y);

                        return 0;
                    }

                case WM_MOUSEMOVE:
                    {
                        g_NextCallback->OnMouseMove(window, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                        return 0;
                    }

                case WM_MOUSEWHEEL:
                    {
                        POINT temp;
                        temp.x = LOWORD(lParam);
                        temp.y = HIWORD(lParam);
                        ScreenToClient(hWnd, &temp);

                        g_NextCallback->OnMouseWheel(window, GET_WHEEL_DELTA_WPARAM(wParam) / 120, temp.x, temp.y);

                        return 0;
                    }

                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    {
                        SetCapture(hWnd);
                        g_NextCallback->OnMouseButtonDown(window, ConvertWin32MouseButton(msg), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                        return 0;
                    }

                case WM_LBUTTONUP:
                case WM_RBUTTONUP:
                case WM_MBUTTONUP:
                    {
                        ReleaseCapture();
                        g_NextCallback->OnMouseButtonUp(window, ConvertWin32MouseButton(msg),  GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

                        return 0;
                    }

                case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDBLCLK:
                case WM_MBUTTONDBLCLK:
                    {
                        int  button = ConvertWin32MouseButton(msg);
                        WORD xPos   = GET_X_LPARAM(lParam);
                        WORD yPos   = GET_Y_LPARAM(lParam);

                        g_NextCallback->OnMouseButtonDown(window, button, xPos, yPos);
                        g_NextCallback->OnMouseButtonDoubleClick(window, button, xPos, yPos);

                        return 0;
                    }



                // TODO: We need to make sure these mouse events are consistent with Linux/X11. Also, look into the raw input API - might need to use that to support all mouse buttons.
                //
                // On Linux: side button, towards rear = 6; towards front = 7.
                case WM_XBUTTONDOWN:
                    {
                        WORD button = GET_XBUTTON_WPARAM(wParam);
                        WORD xPos   = GET_X_LPARAM(lParam);
                        WORD yPos   = GET_Y_LPARAM(lParam);

                        printf("WM_XBUTTONDOWN: %d %d %d\n", button, xPos, yPos);
                        return 1;
                    }

                case WM_XBUTTONUP:
                    {
                        WORD button = GET_XBUTTON_WPARAM(wParam);
                        WORD xPos   = GET_X_LPARAM(lParam);
                        WORD yPos   = GET_Y_LPARAM(lParam);

                        printf("WM_XBUTTONUP: %d %d %d\n", button, xPos, yPos);
                        return 1;
                    }

                case WM_XBUTTONDBLCLK:
                    {
                        WORD button = GET_XBUTTON_WPARAM(wParam);
                        WORD xPos   = GET_X_LPARAM(lParam);
                        WORD yPos   = GET_Y_LPARAM(lParam);

                        printf("WM_XBUTTONDBLCLK: %d %d %d\n", button, xPos, yPos);
                        return 1;
                    }


                // Keyboard messages are a little complicated here because WM_KEYDOWN won't produce Unicode code points. Instead, unicode
                // characters are received in WM_CHAR. To work around this we will delay posting events for printable characters until WM_CHAR.
                //
                // Another issue here is that holding down the Ctrl key will cause the character codes for A, B, C, etc to change. It will change
                // the codes so that A = 1, B = 2, etc. To resolve this, we need to make sure those key codes are handled in WM_KEYDOWN instead
                // of WM_CHAR.

                case WM_KEYDOWN:
                    {
                        // OnKeyDown() is a repeating message, whereas OnKeyPressed() is non-repeating. We actually post both of
                        // these messages from this event.

                        // First we need to convert the key.
                        GTLib::Key key = GetKeyFromWin32VirtualKey(wParam);

                        if (key == GTLib::Keys::Ctrl)
                        {
                            window->flags |= CTRLKeyDown;
                        }

                        if (key != GTLib::Keys::Null)
                        {
                            // Don't post printable characters if the Ctrl key is up.
                            if (!GTLib::IsKeyPrintable(key) || (window->flags & CTRLKeyDown) != 0)
                            {
                                // Only if the key was previously up do we want to post this message...
                                if ((lParam & (1 << 30)) == 0)
                                {
                                    // The key was previously up - we'll post.
                                    g_NextCallback->OnKeyPressed(window, key);
                                }


                                int repeatCount = (lParam & 0xFFFF);
                                for (int i = 0; i < repeatCount; ++i)
                                {
                                    g_NextCallback->OnKeyDown(window, key);
                                }
                            }
                        }

                        return 0;
                    }

                case WM_KEYUP:
                    {
                        GTLib::Key key = GetKeyFromWin32VirtualKey(wParam);

                        if (key == GTLib::Keys::Ctrl)
                        {
                            window->flags &= ~CTRLKeyDown;
                        }

                        g_NextCallback->OnKeyReleased(window, key);
                        g_NextCallback->OnKeyUp(window, key);

                        return 0;
                    }

                case WM_CHAR:
                    {
                        // Some keys will fall-through to this message even if they've been handled by WM_KEYDOWN. We do not want to post
                        // duplicate messages. We will filter them out here.
                        if (wParam != GTLib::Keys::Backspace &&
                            wParam != GTLib::Keys::Return    &&
                            wParam != GTLib::Keys::Delete    &&
                            wParam != GTLib::Keys::Escape    &&
                            wParam != GTLib::Keys::Tab)
                        {
                            bool isCTRLDown = (window->flags & CTRLKeyDown) != 0;
                            if (!(isCTRLDown && wParam < 0x20))       // <-- This filters those annoying translations as a result of the CTRL key being pressed while on an ascii character.
                            {
                                if ((lParam & (1 << 31)) != 0)
                                {
                                    g_NextCallback->OnKeyUp(window, static_cast<GTLib::Key>(wParam));
                                }
                                else
                                {
                                    // We need to check for a key press event. We're going to convert lower case characters to upper case.
                                    if ((lParam & (1 << 30)) == 0)
                                    {
                                        // The key was previously up - we'll post.
                                        WPARAM wParam_OnKeyPress = wParam;
                                        if (wParam >= 'a' && wParam <= 'z')
                                        {
                                            wParam_OnKeyPress -= 0x20;
                                        }

                                        g_NextCallback->OnKeyPressed(window, static_cast<GTLib::Key>(wParam_OnKeyPress));
                                    }


                                    int repeatCount = (lParam & 0xFFFF);
                                    for (int i = 0; i < repeatCount; ++i)
                                    {
                                        g_NextCallback->OnKeyDown(window, static_cast<GTLib::Key>(wParam));
                                    }
                                }
                            }
                        }

                        return 0;
                    }


                case WM_SETFOCUS:
                    {
                        g_NextCallback->OnReceiveFocus(window);
                        
                        return 0;
                    }

                case WM_KILLFOCUS:
                    {
                        g_NextCallback->OnLoseFocus(window);

                        return 0;
                    }


                case WM_TIMER:
                    {
                        g_NextCallback->OnTimer(window, static_cast<uint32_t>(wParam));

                        return 0;
                    }


                ////////////////////////////////////////////////////////////////////////////////////////////
                // Cases below do not post events, but are needed for correct operation of certain things.

                case WM_SETCURSOR:
                    {
                        if ((window->flags & WindowCursorHidden) == 0 && window->cursor != Cursor_None)
                        {
                            SetCursor(GetHCURSOR(window->cursor));
                            Win32::ShowCursor();
                        }
                        else
                        {
                            Win32::HideCursor();
                        }

                        break;      // <-- Don't return 0 or 1 here. It prevents the sizing arrows when resizing the window.
                    }



                default: break;
                }
            }
        }

        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }


	bool StartupWindowManager()
	{
        Win32SystemCursors[Cursor_Arrow]                = ::LoadCursorA(nullptr, IDC_ARROW);
        Win32SystemCursors[Cursor_Beam]                 = ::LoadCursorA(nullptr, IDC_IBEAM);
        Win32SystemCursors[Cursor_Hand]                 = ::LoadCursorA(nullptr, IDC_HAND);
        Win32SystemCursors[Cursor_Cross]                = ::LoadCursorA(nullptr, IDC_CROSS);

        Win32SystemCursors[Cursor_VertDoubleArrow]      = ::LoadCursorA(nullptr, IDC_SIZENS);
        Win32SystemCursors[Cursor_HorzDoubleArrow]      = ::LoadCursorA(nullptr, IDC_SIZEWE);

        Win32SystemCursors[Cursor_SizeArrowTop]         = ::LoadCursorA(nullptr, IDC_SIZENS);
        Win32SystemCursors[Cursor_SizeArrowBottom]      = ::LoadCursorA(nullptr, IDC_SIZENS);
        Win32SystemCursors[Cursor_SizeArrowLeft]        = ::LoadCursorA(nullptr, IDC_SIZEWE);
        Win32SystemCursors[Cursor_SizeArrowRight]       = ::LoadCursorA(nullptr, IDC_SIZEWE);
        Win32SystemCursors[Cursor_SizeArrowTopLeft]     = ::LoadCursorA(nullptr, IDC_SIZENWSE);
        Win32SystemCursors[Cursor_SizeArrowTopRight]    = ::LoadCursorA(nullptr, IDC_SIZENESW);
        Win32SystemCursors[Cursor_SizeArrowBottomLeft]  = ::LoadCursorA(nullptr, IDC_SIZENWSE);
        Win32SystemCursors[Cursor_SizeArrowBottomRight] = ::LoadCursorA(nullptr, IDC_SIZENESW);


        // We'll create our window classes from here, and delete them in ShutdownWindowManager().
        WNDCLASSEXA wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize        = sizeof(wc);
        wc.cbWndExtra    = sizeof(void*);
        wc.lpfnWndProc   = reinterpret_cast<WNDPROC>(WindowProc);
        wc.lpszClassName = WindowClassName;
        wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        if (!RegisterClassExA(&wc))
        {
            return false;
        }

		return true;
	}

	void ShutdownWindowManager()
	{
        UnregisterClassA(WindowClassName, GetModuleHandleA(nullptr));
	}

    
    void StartWindowTimer(WindowHandle window, uint32_t timerID, unsigned int timeoutInMilliseconds)
    {
        SetTimer(window->hWnd, static_cast<UINT_PTR>(timerID), timeoutInMilliseconds, nullptr);
    }

    void StopWindowTimer(WindowHandle window, uint32_t timerID)
    {
        KillTimer(window->hWnd, static_cast<UINT_PTR>(timerID));
    }
    

    MSG g_MSG;
    bool NextWindowEvent(WindowEventCallback &callback, bool wait)
    {
        g_NextCallback = &callback;

        if (wait)
        {
            BOOL result;
            if ((result = GetMessageA(&g_MSG, nullptr, 0, 0)) != 0)
            {
                if (result == -1)       // <-- Error.
                {
                    return false;
                }
            }
            else
            {
                // When we hit this path, 'result' is equal to 0 which means WM_QUIT was received. This is posted when PostQuitMessage() is called.
                return false;
            }
        }
        else
        {
            if (!PeekMessageA(&g_MSG, nullptr, 0, 0, PM_REMOVE))
            {
                return true;
            }
        }


        // At this point we should have valid data in g_MSG. We need to now dispatch the event.
        TranslateMessage(&g_MSG);
        DispatchMessageA(&g_MSG);


        return true;
    }

    void SendWindowEvent(Window* window, uint32_t eventCode, const void* eventData)
    {
        (void)window;
        (void)eventCode;
        (void)eventData;
    }
}
#endif
