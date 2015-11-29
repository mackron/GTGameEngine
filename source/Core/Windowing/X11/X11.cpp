// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifdef __linux__

#include <GTEngine/Core/WindowManagement.hpp>
#include <GTEngine/Core/Windowing/X11/X11.hpp>
#include <GTEngine/Core/Windowing/X11/X11Keys.hpp>
#include <GTEngine/Core/Map.hpp>
#include <GTEngine/Core/Dictionary.hpp>      // <-- For named window properties.
#include <GTEngine/Core/Log.hpp>
#include <GTEngine/Core/Keyboard.hpp>

#include <X11/XKBlib.h>
#include <X11/extensions/xf86vmode.h>
#include <cassert>
#include <climits>

#define DOUBLE_CLICK_TIME 250

namespace GTLib
{
    namespace X11
    {
        // Keeps track of whether or not the next KeyPress event is the result of auto-repeat. This is used in determining
        // if OnKeyPressed should be posted. OnKeyPressed should not be called in response to auto-repeat.
        bool g_IsNextKeyPressAutoRepeated = false;


        /// Converts an X11 mouse button identifier to a GTGUI button identifier.
        MouseButton FromX11MouseButton(unsigned int input)
        {
            switch (input)
            {
            case Button1: return MouseButton_Left;
            case Button2: return MouseButton_Middle;
            case Button3: return MouseButton_Right;

            default: break;
            }

            return MouseButton_Left;
        }

        void DefaultEventHandlerProc(XEvent &e)
        {
            GTLib::WindowEventHandler &eventHandler = GetWindowEventHandler(e.xany.window);

            switch (e.type)
            {
            case ClientMessage:
                {
                    if (X11::IsOnCloseEvent(e))
                    {
                        eventHandler.OnClose();
                    }
                    //else if (X11::IsActivateEvent(e))
                    {

                    }

                    break;
                }

            case PropertyNotify:
                {
                    //printf("Property Notify... %d\n", e.xproperty.atom);
                    if (e.xproperty.atom == XInternAtom(e.xany.display, "_NET_ACTIVE_WINDOW", False))
                    {
                        printf("Active Window Changed...\n");
                    }

                    break;
                }

            case DestroyNotify:
                {
                    //X11::OnDestroyWindow(e.xany.window);
                    break;
                }

            case ConfigureNotify:
                {
                    // TODO: The more correct way to do this is to check what has actually changed since the last time
                    //       the function was called. Will implement this later.
                    int x = e.xconfigure.x;
                    int y = e.xconfigure.y;
                    unsigned int width  = (unsigned int)e.xconfigure.width;
                    unsigned int height = (unsigned int)e.xconfigure.height;

/*
                    if (X11::GetWindowProperty(e.xany.window, "__fullscreen") == (void*)1)
                    {
                        x      = 0;
                        y      = 0;
                        width  = (unsigned int)(size_t)X11::GetWindowProperty(e.xany.window, "__fullscreen_width");
                        height = (unsigned int)(size_t)X11::GetWindowProperty(e.xany.window, "__fullscreen_height");
                    }
*/
                    eventHandler.OnSize(width, height);
                    eventHandler.OnMove(x, y);

                    break;
                }

            case Expose:
                {
                    // TODO:
                    // It's possible that there will be multiple Expose events still waiting to be done. An alternative to
                    // what we're doing here would be to cache all of the expose events and do a re-paint in a single pass.
                    int paintRectLeft   = e.xexpose.x;
                    int paintRectTop    = e.xexpose.y;
                    int paintRectRight  = paintRectLeft + e.xexpose.width;
                    int paintRectBottom = paintRectTop  + e.xexpose.height;

                    eventHandler.OnPaint(paintRectLeft, paintRectTop, paintRectRight, paintRectBottom);

                    break;
                }

            case MotionNotify:
                {
                    // We want a mouse movement to disable a double click event. To do this, we just reset the double click time to 0.
                    X11::SetWindowProperty(e.xany.window, "__lmbdown_lastuptime", (void*)0);
                    X11::SetWindowProperty(e.xany.window, "__mmbdown_lastuptime", (void*)0);
                    X11::SetWindowProperty(e.xany.window, "__rmbdown_lastuptime", (void*)0);

                    eventHandler.OnMouseMove(e.xmotion.x, e.xmotion.y);
                    break;
                }


            // A note about button presses:
            //
            // In VirtualBox, the ButtonPress and ButtonRelease events are posted continuously until the mouse is moved. So, when the mouse
            // button is clicked, a ButtonPress will be posted as expected. However, they will continue to be posted so long as the user holds
            // down the button. The same applies to ButtonRelease.
            //
            // To resolve, we're going to use a property to block the duplicate message.
            case ButtonPress:
                {
                    MouseButton button = MouseButton_Unknown;
                    bool postMessage   = false;

                    switch (e.xbutton.button)
                    {
                    case Button1:
                        {
                            button = MouseButton_Left;
                            if (X11::GetWindowProperty(e.xany.window, "__lmbdown") == 0)
                            {
                                Time lastUpTime = (Time)X11::GetWindowProperty(e.xany.window, "__lmbdown_lastuptime");
                                Time thisTime   = e.xbutton.time;

                                if (thisTime - lastUpTime <= DOUBLE_CLICK_TIME)
                                {
                                    eventHandler.OnMouseButtonDoubleClick(button, e.xbutton.x, e.xbutton.y);
                                }


                                X11::SetWindowProperty(e.xany.window, "__lmbdown", (void*)1);
                                postMessage = true;
                            }

                            break;
                        }

                    case Button2:
                        {
                            button = MouseButton_Middle;
                            if (X11::GetWindowProperty(e.xany.window, "__mmbdown") == 0)
                            {
                                X11::SetWindowProperty(e.xany.window, "__mmbdown", (void*)1);
                                postMessage = true;
                            }

                            break;
                        }

                    case Button3:
                        {
                            button = MouseButton_Right;
                            if (X11::GetWindowProperty(e.xany.window, "__rmbdown") == 0)
                            {
                                X11::SetWindowProperty(e.xany.window, "__rmbdown", (void*)1);
                                postMessage = true;
                            }

                            break;
                        }


                    // Mouse wheel events post Button4 and Button5 press events.
                    case Button4:
                        {
                            eventHandler.OnMouseWheel(1, e.xbutton.x, e.xbutton.y);
                            break;
                        }

                    case Button5:
                        {
                            eventHandler.OnMouseWheel(-1, e.xbutton.x, e.xbutton.y);
                            break;
                        }


                    default: break;
                    }

                    if (postMessage)
                    {
                        eventHandler.OnMouseButtonDown(button, e.xbutton.x, e.xbutton.y);
                    }

                    break;
                }

            case ButtonRelease:
                {
                    MouseButton button = MouseButton_Unknown;
                    bool postMessage   = false;

                    switch (e.xbutton.button)
                    {
                    case Button1:
                        {
                            button = MouseButton_Left;
                            if (X11::GetWindowProperty(e.xany.window, "__lmbdown") != 0)
                            {
                                X11::SetWindowProperty(e.xany.window, "__lmbdown",            (void*)0);
                                X11::SetWindowProperty(e.xany.window, "__lmbdown_lastuptime", (void*)e.xbutton.time);
                                postMessage = true;
                            }

                            break;
                        }

                    case Button2:
                        {
                            button = MouseButton_Middle;
                            if (X11::GetWindowProperty(e.xany.window, "__mmbdown") != 0)
                            {
                                X11::SetWindowProperty(e.xany.window, "__mmbdown",            (void*)0);
                                X11::SetWindowProperty(e.xany.window, "__mmbdown_lastuptime", (void*)e.xbutton.time);
                                postMessage = true;
                            }

                            break;
                        }

                    case Button3:
                        {
                            button = MouseButton_Right;
                            if (X11::GetWindowProperty(e.xany.window, "__rmbdown") != 0)
                            {
                                X11::SetWindowProperty(e.xany.window, "__rmbdown",            (void*)0);
                                X11::SetWindowProperty(e.xany.window, "__rmbdown_lastuptime", (void*)e.xbutton.time);
                                postMessage = true;
                            }

                            break;
                        }

                    default: break;
                    }

                    if (postMessage)
                    {
                        eventHandler.OnMouseButtonUp(button, e.xbutton.x, e.xbutton.y);
                    }

                    break;
                }


            case KeyPress:
                {
                    // Here we need to send OnKeyPressed and OnKeyDown events.

                    // Step 1: We need to convert the X11 key objects. KeySym is a generic symbol for keys.
                    KeySym  tempKeySym;
                    char    tempStr[8];
                    XLookupString(&e.xkey, tempStr, 8, &tempKeySym, nullptr);

                    // Step 2: We need to grab the GTLib key from the X11 key.
                    Key key = FromX11VirtualKey(tempKeySym);

                    // Step 3: Post Events.
                    //
                    // The OnKeyPress event needs to be upper case. Also, we only do this if we are not auto-repeating.
                    if (!g_IsNextKeyPressAutoRepeated)
                    {
                        Key OnKeyPress_key = key;
                        if (key >= 'a' && key <= 'z')
                        {
                            OnKeyPress_key -= 0x20;
                        }
                        eventHandler.OnKeyPressed(OnKeyPress_key);
                        //printf("Down KeySym/Key: %d / %d\n", tempKeySym, key);
                    }

                    // Step 4: Here we post the OnKeyDown() event. This is always called, even for auto-repeated keys. Text boxes
                    //         will use this event when doing text input.
                    eventHandler.OnKeyDown(key, 1);

                    //printf("Down KeySym/Key: %d / %d\n", tempKeySym, key);

                    g_IsNextKeyPressAutoRepeated = false;
                    break;
                }

            case KeyRelease:
                {
                    // We might be auto-repeating. In this case, we don't never want to post an OnKeyReleased or
                    // OnKeyUp event. We can detect this by peeking at the next message. If it is a KeyPress event
                    // of the same key, it's an auto-repeated.
                    XEvent nextEvent;
                    if (XEventsQueued(e.xany.display, QueuedAfterReading))
                    {
                        XPeekEvent(e.xany.display, &nextEvent);
                    }
                    else
                    {
                        nextEvent.type = None;
                    }

                    if (nextEvent.type         == KeyPress    &&
                        nextEvent.xkey.time    == e.xkey.time &&
                        nextEvent.xkey.keycode == e.xkey.keycode)
                    {
                        // The key is auto-repeated.
                        g_IsNextKeyPressAutoRepeated = true;
                    }
                    else
                    {
                        // The key is not auto-repeated. We handle this just like normal.
                        g_IsNextKeyPressAutoRepeated = false;

                        KeySym  tempKeySym;
                        char    tempStr[8];
                        XLookupString(&e.xkey, tempStr, 8, &tempKeySym, nullptr);

                        Key key = FromX11VirtualKey(tempKeySym);


                        // The OnKeyReleased event needs to be upper case.
                        Key OnKeyReleased_key = key;
                        if (key >= 'a' && key <= 'z')
                        {
                            OnKeyReleased_key -= 0x20;
                        }
                        eventHandler.OnKeyReleased(OnKeyReleased_key);

                        eventHandler.OnKeyUp(key);
                    }

                    break;
                }

            case FocusIn:
                {
                    eventHandler.OnReceiveFocus();

                    if (X11::GetWindowProperty(e.xany.window, "__switching_to_fullscreen") == (void*)1)
                    {
                        X11::SetWindowProperty(e.xany.window, "__switching_to_fullscreen", (void*)0);
                    }


                    break;
                }

            case FocusOut:
                {
                    // We will receive this event when the fullscreen mode changes. We don't want to process it in this case.
                    if (X11::GetWindowProperty(e.xany.window, "__switching_to_fullscreen") == 0)
                    {
                        eventHandler.OnLoseFocus();
                    }
                    else
                    {
                        // We'll end up here if we're in the middle of switching to fullscreen mode and we somehow
                        // lost focus. In this case, we want to re-apply focus to the window.
                        XSetInputFocus(e.xany.display, e.xany.window, RevertToNone, CurrentTime);
                    }


                    break;
                }

            case SelectionRequest:
                {
                    X11::HandleEvent_SelectionRequest(e);
                    break;
                }

            case SelectionNotify:
                {
                    break;
                }


            default:
                {
                    break;
                }
            }
        }
    }
}

namespace GTLib
{
    namespace X11
    {
        /// This class holds the display object. We put this in a class so that we can close the display
        /// during it's destructor. The destructor will be called when the application has finished executing.
        struct _DisplayContainer
        {
            _DisplayContainer()
                : display(nullptr),
                  atom_WM_DELETE_WINDOW(0),
                  atom_PRIMARY(0), atom_CLIPBOARD(0),
                  atom_TARGETS(0),
                  atom_TEXT(0),
                  closeDisplay(false), cursorThemeName(nullptr),
                  arrowCursor(0), beamCursor(0), handCursor(0), crossCursor(0),
                  vertDoubleArrowCursor(0), horzDoubleArrowCursor(0),
                  sizeArrowTopCursor(0), sizeArrowBottomCursor(0), sizeArrowLeftCursor(0), sizeArrowRightCursor(0),
                  sizeArrowTopLeftCursor(0), sizeArrowTopRightCursor(0), sizeArrowBottomLeftCursor(0), sizeArrowBottomRightCursor(0),
                  clipboardTextPRIMARY(), clipboardTextCLIPBOARD()
            {
            }

            ~_DisplayContainer()
            {
            }

            void Initialize(Display *display)
            {
                if (display == nullptr)
                {
                    this->display      = XOpenDisplay(nullptr);
                    this->closeDisplay = true;
                }
                else
                {
                    this->display = display;
                }

                this->atom_WM_DELETE_WINDOW  = XInternAtom(this->display, "WM_DELETE_WINDOW", True);
                this->atom_PRIMARY           = XInternAtom(this->display, "PRIMARY",          True);
                this->atom_CLIPBOARD         = XInternAtom(this->display, "CLIPBOARD",        True);
                this->atom_TARGETS           = XInternAtom(this->display, "TARGETS",          True);
                this->atom_TEXT              = XInternAtom(this->display, "TEXT",             True);

                this->cursorThemeName = XcursorGetTheme(this->display);

                this->arrowCursor   = XcursorShapeLoadCursor(this->display, XC_arrow);
                this->beamCursor    = XcursorShapeLoadCursor(this->display, XC_xterm);
                this->handCursor    = XcursorShapeLoadCursor(this->display, XC_hand1);
                this->crossCursor   = XcursorShapeLoadCursor(this->display, XC_tcross);

                this->vertDoubleArrowCursor = XcursorShapeLoadCursor(this->display, XC_sb_v_double_arrow);
                this->horzDoubleArrowCursor = XcursorShapeLoadCursor(this->display, XC_sb_h_double_arrow);

                this->sizeArrowTopCursor         = XcursorShapeLoadCursor(this->display, XC_top_side);
                this->sizeArrowBottomCursor      = XcursorShapeLoadCursor(this->display, XC_bottom_side);
                this->sizeArrowLeftCursor        = XcursorShapeLoadCursor(this->display, XC_left_side);
                this->sizeArrowRightCursor       = XcursorShapeLoadCursor(this->display, XC_right_side);
                this->sizeArrowTopLeftCursor     = XcursorShapeLoadCursor(this->display, XC_top_left_corner);
                this->sizeArrowTopRightCursor    = XcursorShapeLoadCursor(this->display, XC_top_right_corner);
                this->sizeArrowBottomLeftCursor  = XcursorShapeLoadCursor(this->display, XC_bottom_left_corner);
                this->sizeArrowBottomRightCursor = XcursorShapeLoadCursor(this->display, XC_bottom_right_corner);
            }

            void Uninitialize()
            {
                if (this->closeDisplay)
                {
                    XCloseDisplay(this->display);
                }



                // TODO: Destroy cursor
            }

            /// The display.
            Display *display;

            /// The X11 Atom we need to use for handling when the close button is hit.
            Atom atom_WM_DELETE_WINDOW;

            Atom atom_PRIMARY;
            Atom atom_CLIPBOARD;
            Atom atom_TARGETS;
            Atom atom_TEXT;

            /// Keeps track of whether or not the display needs to be closed.
            bool closeDisplay;

            /// The name of the cursor theme. Do NOT delete this in the destructor!
            const char* cursorThemeName;

            // The list of cursor images we might need.
            ::Cursor arrowCursor;
            ::Cursor beamCursor;
            ::Cursor handCursor;
            ::Cursor crossCursor;

            ::Cursor vertDoubleArrowCursor;
            ::Cursor horzDoubleArrowCursor;

            ::Cursor sizeArrowTopCursor;
            ::Cursor sizeArrowBottomCursor;
            ::Cursor sizeArrowLeftCursor;
            ::Cursor sizeArrowRightCursor;
            ::Cursor sizeArrowTopLeftCursor;
            ::Cursor sizeArrowTopRightCursor;
            ::Cursor sizeArrowBottomLeftCursor;
            ::Cursor sizeArrowBottomRightCursor;

            /// The clipboard text on the PRIMARY target.
            GTLib::String clipboardTextPRIMARY;

            /// The clipboard text on the CLIPBOARD target.
            GTLib::String clipboardTextCLIPBOARD;

        private:    // No copying.
            _DisplayContainer(const _DisplayContainer &);
            _DisplayContainer & operator=(const _DisplayContainer &);

        }DisplayContainer;

        /**
        *   \brief  Structure containing information about each window.
        *
        *   One of these structures is created for each call to OnWindowCreated(). It is deleted with the
        *   OnWindowDestroyed() function.
        */
        struct WindowMetadata
        {
            WindowMetadata()
                : properties()
            {
            }

            /// The dictionary containing the properties of the window.
            GTLib::Dictionary<void*> properties;


        private:    // No copying.
            WindowMetadata(const WindowMetadata &);
            WindowMetadata & operator=(const WindowMetadata &);
        };

        /// The map containing the window data for every window that posted the GTLib::X11::OnWindowCreated() event.
        GTLib::Map< ::Window, WindowMetadata*> Windows;


        /**
        *   \brief  Helper function for retrieving the metadata of the given window pair.
        */
        WindowMetadata * GetWindowMetadata(::Window window)
        {
            auto iMetadata = Windows.Find(window);
            if (iMetadata != nullptr)
            {
                return iMetadata->value;
            }

            return nullptr;
        }

        void Initialize(Display *display)
        {
            DisplayContainer.Initialize(display);
        }

        void Uninitialize()
        {
            DisplayContainer.Uninitialize();
        }

/*
        Display * GetDisplay()
        {
            return DisplayContainer.display;
        }
        */

        void OnCreateWindow(::Window window)
        {
            // We just add some metadata for the given window, if it doesn't already exist.
            if (Windows.Find(window) == nullptr)
            {
                auto metadata = new WindowMetadata;
                Windows.Add(window, metadata);
            }

            // When a window is created, we want to set the WM_DELETE_WINDOW protocol so that we can use the
            // X11::IsOnCloseEvent() function in response to the ClientMessage X event. The WM_DELETE_WINDOW
            // Atom is created when the display metadata is constructed.
            XSetWMProtocols(GetX11Display(), window, &DisplayContainer.atom_WM_DELETE_WINDOW, 1);


            // We need to create some cursor objects for this window. We attach them to the window as properties.
            const char invisiblePixmapData = 0;
            XColor black;
            black.red   = 0;
            black.green = 0;
            black.blue  = 0;

            ::Pixmap invisiblePixmap = XCreateBitmapFromData(GetX11Display(), window, &invisiblePixmapData, 1, 1);
            ::Cursor invisibleCursor = XCreatePixmapCursor(GetX11Display(), invisiblePixmap, invisiblePixmap, &black, &black, 0, 0);

            X11::SetWindowProperty(window, "__invisiblecursorpixmap", (void*)invisiblePixmap);
            X11::SetWindowProperty(window, "__invisiblecursor",       (void*)invisibleCursor);


            // The cursor is visible to begin with, so we will mark it as such.
            X11::SetWindowProperty(window, "__cursorvisible", (void*)1);
        }

        void OnDestroyWindow(::Window window)
        {
            // We need to delete our cursors.
            XFreeCursor(GetX11Display(), (::Cursor)X11::GetWindowProperty(window, "__invisiblecursor"));
            XFreePixmap(GetX11Display(), (::Pixmap)X11::GetWindowProperty(window, "__invisiblecursorpixmap"));

            // The opposite of OnWindowCreated()...
            auto iMetadata = Windows.Find(window);
            if (iMetadata != nullptr)
            {
                delete iMetadata->value;
            }

            Windows.RemoveByKey(window);
        }

        ::Window CreateWindow(XVisualInfo *vi, Colormap colourmap, bool overrideRedirect)
        {
            // FIXME: This will crash when vi is nullptr.

            auto display = GetX11Display();

            XSetWindowAttributes wa;
            wa.colormap          = colourmap;
            wa.border_pixel      = 0;
            wa.event_mask        = StructureNotifyMask | SubstructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonMotionMask | FocusChangeMask | PropertyChangeMask;
            wa.override_redirect = overrideRedirect;

            ::Window window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0, 1, 1,     // <-- Windows can not have dimensions of 0 in X11. We stick with dimensions of 1.
                                            0, vi->depth, InputOutput, vi->visual,
                                            CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &wa);

            // Now we just let the X11 namespace know about the new window.
            X11::OnCreateWindow(window);

            return window;
        }

        void DestroyWindow(::Window window)
        {
            SetWindowProperty(window, "__eventhandler", nullptr);

            XDestroyWindow(GetX11Display(), window);

            // Everything needs to know about this...
            X11::OnDestroyWindow(window);
        }

        void SetWindowProperty(::Window window, const char* propertyName, void* propertyValue)
        {
            auto data = GetWindowMetadata(window);
            if (data != nullptr)
            {
                auto item = data->properties.Find(propertyName);
                if (item != nullptr)
                {
                    item->value = propertyValue;
                }
                else
                {
                    data->properties.Add(propertyName, propertyValue);
                }
            }
        }

        void* GetWindowProperty(::Window window, const char *propertyName)
        {
            WindowMetadata *data = GetWindowMetadata(window);
            if (data != nullptr)
            {
                auto iProperty = data->properties.Find(propertyName);
                if (iProperty != nullptr)
                {
                    return iProperty->value;
                }
            }

            return nullptr;
        }

        void CopyWindowProperties(::Window source, ::Window dest)
        {
            auto sourceMetadata = GetWindowMetadata(source);
            if (sourceMetadata != nullptr)
            {
                for (size_t i = 0; i < sourceMetadata->properties.count; ++i)
                {
                    auto iProperty = sourceMetadata->properties.buffer[i];

                    // Some properties don't want to be copied.
                    if (!GTLib::Strings::Equal(iProperty->key, "__invisiblecursorpixmap") &&
                        !GTLib::Strings::Equal(iProperty->key, "__invisiblecursor"))
                    {
                        SetWindowProperty(dest, iProperty->key, iProperty->value);
                    }
                }
            }
        }


        void CopyWindowTitle(::Window source, ::Window dest)
        {
            XTextProperty windowName;
            XGetWMName(GetX11Display(), source, &windowName);
            XSetWMName(GetX11Display(), dest,   &windowName);
        }


        bool IsOnCloseEvent(XEvent &e)
        {
            // We first need to check if we have metadata attached to the window. If so, it will contain the atom we need to check.
            if ((Atom)e.xclient.data.l[0] == DisplayContainer.atom_WM_DELETE_WINDOW)
            {
                return true;
            }

            return false;
        }

        bool IsActiveWindowEvent(XEvent &e)
        {
            if ((Atom)e.xclient.message_type == XInternAtom(GetX11Display(), "_NET_ACTIVE_WINDOW", True))
            {
                return true;
            }

            return false;
        }


        WindowEventHandler & GetWindowEventHandler(::Window window)
        {
            auto result = reinterpret_cast<WindowEventHandler *>(GetWindowProperty(window, "__eventhandler"));
            if (result != nullptr)
            {
                return *result;
            }

            return WindowEventHandler::GetDefault();
        }

        void SetWindowEventHandler(::Window window, WindowEventHandler &eventHandler)
        {
            SetWindowProperty(window, "__eventhandler", &eventHandler);
        }

        void DefaultEventHandler(XEvent &)
        {
            // Empty for now...
        }


        int FindBestVideoMode(int screen, unsigned int &width, unsigned int &height)
        {
            int modeCount;
            XF86VidModeModeInfo** modes;

            if (XF86VidModeGetAllModeLines(GetX11Display(), screen, &modeCount, &modes))
            {
                int bestMode  = -1;
                int bestMatch = INT_MAX;
                for(int i = 0; i < modeCount; i ++)
                {
                    int match = (width  - modes[i]->hdisplay) *
                                (width  - modes[i]->hdisplay) +
                                (height - modes[i]->vdisplay) *
                                (height - modes[i]->vdisplay);

                    if(match < bestMatch)
                    {
                        bestMatch = match;
                        bestMode  = i;
                    }
                }

                width  = modes[bestMode]->hdisplay;
                height = modes[bestMode]->vdisplay;

                XFree(modes);

                return bestMode;
            }

            return -1;
        }


        // This map stores default video modes for screens. The key is the screen.
        static GTLib::Map<int, XF86VidModeModeInfo> DefaultVideoModes;

        void SwitchVideoMode(int screen, int mode)
        {
            if (mode >= 0)      // <-- FindBestVideoMode() can return -1 on error. We handle that here.
            {
                int modeCount;
                XF86VidModeModeInfo** modes;

                if (XF86VidModeGetAllModeLines(GetX11Display(), screen, &modeCount, &modes))
                {
                    if (mode < modeCount)
                    {
                        XF86VidModeSwitchToMode(GetX11Display(), screen, modes[mode]);
                        XF86VidModeSetViewPort(GetX11Display(), screen, 0, 0);

                        // If we don't have the default mode for this screen yet, we'll need it. We store this in a static map.
                        if (!DefaultVideoModes.DoesItemExist(screen))
                        {
                            DefaultVideoModes.Add(screen, *modes[0]);
                        }

                        XFree(modes);
                        XFlush(GetX11Display());
                    }
                }
            }
        }

        void SwitchToBestVideoMode(int screen, unsigned int &width, unsigned int &height)
        {
            SwitchVideoMode(screen, FindBestVideoMode(screen, width, height));
        }

        void RestoreVideoMode(int screen)
        {
            auto iVideoMode = DefaultVideoModes.Find(screen);
            if (iVideoMode != nullptr)
            {
                XF86VidModeSwitchToMode(GetX11Display(), screen, &iVideoMode->value);
                XF86VidModeSetViewPort(GetX11Display(), screen, 0, 0);

                XFlush(GetX11Display());
            }
        }



        int MapNotifyPredicate(Display*, XEvent* e, char* arg)
        {
            return e->type == MapNotify && e->xmap.window == (::Window)arg;
        }

        void MapWindow(::Window window)
        {
            XMapRaised(GetX11Display(), window);

            XEvent e;
            XIfEvent(GetX11Display(), &e, MapNotifyPredicate, (char*)window);
        }
        
        void MapWindowMinimized(::Window window)
        {
            XMapWindow(GetX11Display(), window);
            XIconifyWindow(GetX11Display(), window, DefaultScreen(GetX11Display()));

            XEvent e;
            XIfEvent(GetX11Display(), &e, MapNotifyPredicate, (char*)window);
        }
        
        void MapWindowMaximized(::Window window)
        {
            XMapRaised(GetX11Display(), window);
            
            
            Atom Atom_NET_WM_STATE                = XInternAtom(GetX11Display(), "_NET_WM_STATE",                True);
            Atom Atom_NET_WM_STATE_MAXIMIZED_VERT = XInternAtom(GetX11Display(), "_NET_WM_STATE_MAXIMIZED_VERT", True);
            Atom Atom_NET_WM_STATE_MAXIMIZED_HORZ = XInternAtom(GetX11Display(), "_NET_WM_STATE_MAXIMIZED_HORZ", True);

            XEvent xev;
            memset(&xev, 0, sizeof(xev));
            xev.type                 = ClientMessage;
            xev.xclient.window       = window;
            xev.xclient.message_type = Atom_NET_WM_STATE;
            xev.xclient.format       = 32;
            xev.xclient.data.l[0]    = 1;
            xev.xclient.data.l[1]    = Atom_NET_WM_STATE_MAXIMIZED_VERT;
            xev.xclient.data.l[2]    = Atom_NET_WM_STATE_MAXIMIZED_HORZ;
            
            XSendEvent(GetX11Display(), DefaultRootWindow(GetX11Display()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
            XFlush(GetX11Display());

            

            XEvent e;
            XIfEvent(GetX11Display(), &e, MapNotifyPredicate, (char*)window);
        }

        void UnmapWindow(::Window window)
        {
            XUnmapWindow(GetX11Display(), window);
        }


        ::Window GetFirstWindow()
        {
            if (Windows.count > 0)
            {
                return Windows.buffer[0]->key;
            }

            return 0;
        }


        void SetCursor(::Window window, GTLib::Cursor cursor)
        {
            SetWindowProperty(window, "__cursor", (void*)cursor);

            if (cursor == Cursor_Arrow)
            {
                XUndefineCursor(GetX11Display(), window);
            }
            else
            {
                XDefineCursor(GetX11Display(), window, GetX11Cursor(cursor));
            }

            XSync(GetX11Display(), False);
        }

        ::Cursor GetX11Cursor(GTLib::Cursor cursor)
        {
            switch (cursor)
            {
            case Cursor_Arrow:                return DisplayContainer.arrowCursor;
            case Cursor_Beam:                 return DisplayContainer.beamCursor;
            case Cursor_Hand:                 return DisplayContainer.handCursor;
            case Cursor_Cross:                return DisplayContainer.crossCursor;

            case Cursor_VertDoubleArrow:      return DisplayContainer.vertDoubleArrowCursor;
            case Cursor_HorzDoubleArrow:      return DisplayContainer.horzDoubleArrowCursor;

            case Cursor_SizeArrowTop:         return DisplayContainer.sizeArrowTopCursor;
            case Cursor_SizeArrowBottom:      return DisplayContainer.sizeArrowBottomCursor;
            case Cursor_SizeArrowLeft:        return DisplayContainer.sizeArrowLeftCursor;
            case Cursor_SizeArrowRight:       return DisplayContainer.sizeArrowRightCursor;
            case Cursor_SizeArrowTopLeft:     return DisplayContainer.sizeArrowTopLeftCursor;
            case Cursor_SizeArrowTopRight:    return DisplayContainer.sizeArrowTopLeftCursor;
            case Cursor_SizeArrowBottomLeft:  return DisplayContainer.sizeArrowTopLeftCursor;
            case Cursor_SizeArrowBottomRight: return DisplayContainer.sizeArrowTopLeftCursor;

            case Cursor_None:
            case Cursor_Count:
            default: break;
            }

            return DisplayContainer.arrowCursor;
        }


        /////////////////////////////////////////////
        // Clipboard.

        void SetClipboardText(const char* text, Atom target)
        {
            X11::SetClipboardText(text, -1, target);
        }

        void SetClipboardText(const char* text, ptrdiff_t sizeInTs, Atom target)
        {
            if (target == X11::GetAtomPRIMARY())
            {
                DisplayContainer.clipboardTextPRIMARY.Assign(text, sizeInTs);
            }
            else if (target == X11::GetAtomCLIPBOARD())
            {
                DisplayContainer.clipboardTextCLIPBOARD.Assign(text, sizeInTs);
            }
        }

        const char* GetClipboardText(Atom target)
        {
            if (target == X11::GetAtomPRIMARY())
            {
                return DisplayContainer.clipboardTextPRIMARY.c_str();
            }
            else if (target == X11::GetAtomCLIPBOARD())
            {
                return DisplayContainer.clipboardTextCLIPBOARD.c_str();
            }

            return "";
        }

        size_t GetClipboardTextLength(Atom target)
        {
            if (target == X11::GetAtomPRIMARY())
            {
                return DisplayContainer.clipboardTextPRIMARY.GetLength();
            }
            else if (target == X11::GetAtomCLIPBOARD())
            {
                return DisplayContainer.clipboardTextCLIPBOARD.GetLength();
            }

            return 0;
        }

        Atom GetAtomTARGETS()
        {
            return DisplayContainer.atom_TARGETS;
        }

        Atom GetAtomPRIMARY()
        {
            return XA_PRIMARY;
            //return DisplayContainer.atom_PRIMARY;
        }

        Atom GetAtomCLIPBOARD()
        {
            return DisplayContainer.atom_CLIPBOARD;
        }

        Atom GetAtomTEXT()
        {
            return DisplayContainer.atom_TEXT;
        }

        void HandleEvent_SelectionRequest(const XEvent &e)
        {
            // This event will be called when something is requesting our clipboard or selection data.
            XEvent s;
            s.xselection.type      = SelectionNotify;
            s.xselection.requestor = e.xselectionrequest.requestor;
            s.xselection.selection = e.xselectionrequest.selection;
            s.xselection.target    = e.xselectionrequest.target;
            s.xselection.property  = e.xselectionrequest.property;
            s.xselection.time      = e.xselectionrequest.time;

            if (e.xselectionrequest.target == X11::GetAtomTARGETS())
            {
                // We're posting what we're offering.
                uint32_t targets[] = {static_cast<uint32_t>(X11::GetAtomTARGETS()), static_cast<uint32_t>(X11::GetAtomTEXT())};
                XChangeProperty(e.xany.display, s.xselection.requestor, s.xselection.property, XA_ATOM, 32, PropModeReplace, (unsigned char*)targets, 2);
            }
            else
            {
                //if (e.xselectionrequest.target == X11::GetAtomPRIMARY() || e.xselectionrequest.target == X11::GetAtomCLIPBOARD())
                if (e.xselectionrequest.target == X11::GetAtomTEXT())
                {
                    XChangeProperty(e.xany.display, s.xselection.requestor, s.xselection.property, s.xselection.target, 8, PropModeReplace,
                        reinterpret_cast<const unsigned char*>(X11::GetClipboardText(s.xselection.property)), static_cast<int>(X11::GetClipboardTextLength(s.xselection.property)));
                }
                else
                {
                    // We don't support whatever it is you're after.
                    s.xselection.property = None;
                }
            }

            XSendEvent(e.xany.display, e.xselectionrequest.requestor, True, 0, &s);
            XFlush(GetX11Display());
        }


        /////////////////////////////////////////////
        // Properties.

        void GetWindowProperty(::Window window, Atom property, Property &valueOut)
        {
            unsigned long  dataSizeInBytes;
            unsigned char* tempData;
            XGetWindowProperty(GetX11Display(), window, property, 0, 0, False, AnyPropertyType,
                &valueOut.type, &valueOut.format, &valueOut.itemCount, &dataSizeInBytes, &tempData);
            XFree(tempData);

            valueOut.data.Clear();
            valueOut.data.Resize(dataSizeInBytes);
            valueOut.data.count = dataSizeInBytes;

            XGetWindowProperty(GetX11Display(), window, property, 0, dataSizeInBytes, False, AnyPropertyType,
                &valueOut.type, &valueOut.format, &valueOut.itemCount, &dataSizeInBytes, &tempData);

            memcpy(valueOut.data.buffer, tempData, valueOut.data.count);
            XFree(tempData);
        }
    }
}

namespace GTLib
{
    XEvent g_event;
    bool PumpNextWindowEvent(bool wait)
    {
        auto display = GetX11Display();

        // We need the number of events that are waiting. If this is more than 1, there are more events
        // needing to be processed, meaning we need to return false, in that case.
        int eventCount = XPending(display);
        if (wait)
        {
            XNextEvent(display, &g_event);
        }
        else
        {
            // Only if we have events do we want to call XNextEvent here. If we don't do this, it will block, which
            // is not what we want (wait == false).
            if (eventCount > 0)
            {
                XNextEvent(display, &g_event);
            }
            else
            {
                g_event.type = None;
            }
        }

        // When using this function, we're going to assume the window has been managed from the GTGUI::X11 namespace. If this
        // is not the case, the application will need to pump it's own events.
        if (g_event.type != None)
        {
            X11::DefaultEventHandlerProc(g_event);
        }

        return eventCount > 1;
    }

    void HideMouse()
    {
    }

    void ShowMouse()
    {
    }
}

#endif
