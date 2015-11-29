// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifdef __linux__

#include <GTEngine/Core/Window.hpp>
#include <GTEngine/Core/WindowManagement.hpp>
#include <GTEngine/Core/Windowing/X11/X11.hpp>
#include <GTEngine/Core/Strings/Size.hpp>

#include <cstdio>


namespace GTLib
{
    // These externs are defined in WindowManagement.cpp
    extern Atom g_atom_WM_DELETE_WINDOW;
    extern Atom g_atom_GT_WINDOW;
    
    // Flags.
    extern const uint32_t DeleteColormap;
    extern const uint32_t IsFullScreen;
    extern const uint32_t IsNextKeyPressAutoRepeated;
    
    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height)
    {
        return CreateWindow(x, y, width, height, nullptr, 0);
    }
    
    WindowHandle CreateWindow(int x, int y, unsigned int width, unsigned int height, XVisualInfo* vi, Colormap colormap)
    {
        auto display = GetX11Display();
        assert(display != nullptr);
        {
            auto window = new _Window;
            window->display = display;
            window->vi      = vi;
            
            if (colormap != 0)
            {
                window->colormap = colormap;
            }
            else
            {
                if (vi != nullptr)
                {
                    window->colormap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
                    window->flags   |= DeleteColormap;
                }
                else
                {
                    window->colormap = 0;
                }
            }
            
            
            // This is the actual window creation.
            XSetWindowAttributes wa;
            wa.colormap          = window->colormap;
            wa.background_pixel  = 0;
            wa.border_pixel      = 0;
            wa.event_mask        = StructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonMotionMask | FocusChangeMask | PropertyChangeMask;
            wa.override_redirect = false;
            
            int           depth          = (vi != nullptr) ? vi->depth  : CopyFromParent;
            Visual*       visual         = (vi != nullptr) ? vi->visual : CopyFromParent;
            ::Window      parent         = (vi != nullptr) ? RootWindow(display, vi->screen) : DefaultRootWindow(display);
            unsigned long attributesMask = CWBackPixel | CWBorderPixel | CWEventMask | CWOverrideRedirect;
            if (wa.colormap != 0)
            {
                attributesMask |= CWColormap;
            }
            
            window->window = XCreateWindow(display, parent, x, y, width, height, 0, depth, InputOutput, visual, attributesMask, &wa);
            
            
            // Set a few window manager protocols.
            XSetWMProtocols(window->display, window->window, &g_atom_WM_DELETE_WINDOW, 1);
            
            
            // We need to map our window handle to the X11 window so we can retrieve it from the X11 window when the need arises.
            XChangeProperty(window->display, window->window, g_atom_GT_WINDOW, XA_INTEGER, 8, PropModeReplace, reinterpret_cast<unsigned char*>(&window), sizeof(window));
            
            return window;
        }
        
        return nullptr;
    }
    
    
    void DeleteWindow(WindowHandle window)
    {
        if (window != nullptr)
        {
            if (window->colormap != 0 && (window->flags & DeleteColormap) != 0)
            {
                XFreeColormap(window->display, window->colormap);
            }
            
            delete window;
        }
    }
    
    
    int MapNotifyPredicate(Display*, XEvent* e, char* arg)
    {
        return e->type == MapNotify && e->xmap.window == reinterpret_cast<::Window>(arg);
    }
    
    int UnmapNotifyPredicate(Display*, XEvent* e, char* arg)
    {
        return e->type == UnmapNotify && e->xmap.window == reinterpret_cast<::Window>(arg);
    }
    
    void ShowWindow(WindowHandle window)
    {
        if (window != nullptr)
        {
            XMapRaised(window->display, window->window);

            XEvent e;
            XIfEvent(window->display, &e, MapNotifyPredicate, reinterpret_cast<char*>(window->window));
        }
    }
    
    void HideWindow(WindowHandle window)
    {
        if (window != nullptr)
        {
            XUnmapWindow(window->display, window->window);
            
            XEvent e;
            XIfEvent(window->display, &e, UnmapNotifyPredicate, reinterpret_cast<char*>(window->window));
        }
    }
    
    
    void SetWindowTitle(WindowHandle window, const char* title)
    {
        // All of this just to set the title. This is ridiculous...
        XTextProperty windowName;
        windowName.value    = (unsigned char*)(title);
        windowName.encoding = XA_STRING;
        windowName.format   = 8;
        windowName.nitems   = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(title));

        XSetWMName(window->display, window->window, &windowName);
    }
    
    
    void GetWindowPosition(WindowHandle window, int &xOut, int &yOut)
    {
        ::Window devnull;
        XTranslateCoordinates(window->display, window->window, DefaultRootWindow(window->display), 0, 0, &xOut, &yOut, &devnull);
    }
    
    void SetWindowPosition(WindowHandle window, int x, int y)
    {
        XMoveWindow(window->display, window->window, x, y);
        XSync(window->display, False);
    }
    
    
    void GetWindowSize(WindowHandle window, unsigned int &widthOut, unsigned int &heightOut)
    {
        XWindowAttributes wa;
        XGetWindowAttributes(window->display, window->window, &wa);

        widthOut  = static_cast<unsigned int>(wa.width);
        heightOut = static_cast<unsigned int>(wa.height);
    }
    
    void SetWindowSize(WindowHandle window, unsigned int width, unsigned int height)
    {
        XResizeWindow(window->display, window->window, width, height);
        XSync(window->display, False);
    }
}


#define THIS_WINDOW  (this->iwo.window)
#define THIS_DISPLAY (GetX11Display())

namespace GTLib
{
    // Structure used for changing the decoration of the window for fullscreen mode.
    struct MotifHints
    {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long          inputMode;
        unsigned long status;
    };


    Window::Window()
        : iwo()
    {
        this->iwo.window = X11::CreateWindow(nullptr, 0);
        
        X11::SetWindowProperty(this->iwo.window, "__windowGTPtr", this);
    }

    Window::Window(const InternalWindowObjects &iwo)
        : iwo(iwo)
    {
        if (iwo.window == 0)
        {
            this->iwo.window = X11::CreateWindow(iwo.vi, iwo.colormap);
        }
        else
        {
            // We need to make sure this window is managed by the X11 namespace.
            X11::OnCreateWindow(THIS_WINDOW);
        }
        
        X11::SetWindowProperty(this->iwo.window, "__windowGTPtr", this);
    }

    Window::~Window()
    {
        this->DisableFullscreen();
        X11::DestroyWindow(this->iwo.window);
    }

    WindowEventHandler & Window::GetEventHandler()
    {
        return X11::GetWindowEventHandler(THIS_WINDOW);
    }

    void Window::SetEventHandler(WindowEventHandler &handler)
    {
        X11::SetWindowEventHandler(THIS_WINDOW, handler);
    }

    void Window::GetPosition(int &x, int &y)
    {
        ::Window devnull;
        XTranslateCoordinates(THIS_DISPLAY, THIS_WINDOW, DefaultRootWindow(THIS_DISPLAY), 0, 0, &x, &y, &devnull);
    }

    void Window::SetPosition(int x, int y)
    {
        XMoveWindow(THIS_DISPLAY, THIS_WINDOW, x, y);
        XSync(THIS_DISPLAY, False);
    }

    void Window::GetSize(unsigned int &width, unsigned int &height)
    {
        XWindowAttributes wa;
        XGetWindowAttributes(THIS_DISPLAY, THIS_WINDOW, &wa);

        width  = (unsigned int)wa.width;
        height = (unsigned int)wa.height;
    }

    void Window::SetSize(unsigned int width, unsigned int height)
    {
        XResizeWindow(THIS_DISPLAY, THIS_WINDOW, width, height);
        XSync(THIS_DISPLAY, False);
    }

    void Window::SetTitle(const char* title)
    {
        // All of this just to set the title. This is kinda ridiculous...
        XTextProperty windowName;
        windowName.value    = (unsigned char*)(title);
        windowName.encoding = XA_STRING;
        windowName.format   = 8;
        windowName.nitems   = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(title));

        XSetWMName(THIS_DISPLAY, THIS_WINDOW, &windowName);
    }

    void Window::Show(WindowShowType type)
    {
        switch (type)
        {
        case WindowShowType_Normal:
            {
                X11::MapWindow(THIS_WINDOW);
                break;
            }
            
        case WindowShowType_Minimized:
            {
                X11::MapWindowMinimized(THIS_WINDOW);
                break;
            }
            
        case WindowShowType_Maximized:
            {
                X11::MapWindowMaximized(THIS_WINDOW);
                break;
            }
            
        default:
            {
                assert(false);
                break;
            }
        }
    }

    void Window::Redraw(int left, int top, int right, int bottom)
    {
        XClearArea(THIS_DISPLAY, THIS_WINDOW,
                   left, top,
                   static_cast<unsigned int>(right - left), static_cast<unsigned int>(bottom - top),
                   true);
    }

    void Window::SetCursor(Cursor cursor)
    {
        X11::SetCursor(THIS_WINDOW, cursor);
    }

    void Window::HideCursor()
    {
        if (X11::GetWindowProperty(THIS_WINDOW, "__cursorvisible") != 0)
        {
            X11::SetWindowProperty(THIS_WINDOW, "__cursorvisible", (void*)0);

            ::Cursor invisibleCursor = (::Cursor)X11::GetWindowProperty(THIS_WINDOW, "__invisiblecursor");
            XDefineCursor(THIS_DISPLAY, THIS_WINDOW, invisibleCursor);
            XSync(THIS_DISPLAY, False);
        }
    }

    void Window::ShowCursor()
    {
        if (X11::GetWindowProperty(THIS_WINDOW, "__cursorvisible") == 0)
        {
            X11::SetWindowProperty(THIS_WINDOW, "__cursorvisible", (void*)1);
            XUndefineCursor(THIS_DISPLAY, THIS_WINDOW);
            XSync(THIS_DISPLAY, False);
        }
    }


    void Window::GetMousePosition(int &x, int &y)
    {
        int dummyA, dummyB;
        unsigned int dummyMask;

        ::Window dummyRootWindow, dummyChildWindow;

        if (!XQueryPointer(THIS_DISPLAY, THIS_WINDOW, &dummyRootWindow, &dummyChildWindow, &dummyA, &dummyB, &x, &y, &dummyMask))
        {
            x = 0;
            y = 0;
        }
    }

    void Window::SetMousePosition(int x, int y)
    {
        XWarpPointer(THIS_DISPLAY, None, THIS_WINDOW, 0, 0, 1, 1, x, y);
        XSync(THIS_DISPLAY, False);
    }

    // The typical behaviour of a fullscreen app is that is will disable fullscreen when it loses focus. A problem
    // with our current system (which will probably change anyway) is that going fullscreen will destroy the original
    // window, which will then cause it to lose focus and thus try to disable fullscreen. To remedy this, we'e just
    // going to set a flag which tells the window not to disable fullscreen if we're already in the process of
    // switching.

    void Window::EnableFullscreen(unsigned int width, unsigned int height)
    {
        (void)width;
        (void)height;
        
        if (!this->IsFullscreen())
        {
            // This part is strange and really needs to be changed. We will set this back to 0 when the window
            // receives FocusOut.
            X11::SetWindowProperty(THIS_WINDOW, "__fullscreen",              (void*)1);
            X11::SetWindowProperty(THIS_WINDOW, "__switching_to_fullscreen", (void*)1);

            // Here we are going to retrieve the rectangle of the window and save it as a property. These will be used when restoring the window back from fullscreen.
            int originalX;
            int originalY;
            unsigned int originalWidth;
            unsigned int originalHeight;

            this->GetPosition(originalX, originalY);
            this->GetSize(originalWidth, originalHeight);

            X11::SetWindowProperty(THIS_WINDOW, "__x",      (void*)(size_t)originalX);
            X11::SetWindowProperty(THIS_WINDOW, "__y",      (void*)(size_t)originalY);
            X11::SetWindowProperty(THIS_WINDOW, "__width",  (void*)(size_t)originalWidth);
            X11::SetWindowProperty(THIS_WINDOW, "__height", (void*)(size_t)originalHeight);



            Atom Atom_NET_WM_STATE            = XInternAtom(THIS_DISPLAY, "_NET_WM_STATE",            True);
            Atom Atom_NET_WM_STATE_FULLSCREEN = XInternAtom(THIS_DISPLAY, "_NET_WM_STATE_FULLSCREEN", True);

            XEvent xev;
            memset(&xev, 0, sizeof(xev));
            xev.type                 = ClientMessage;
            xev.xclient.window       = THIS_WINDOW;
            xev.xclient.message_type = Atom_NET_WM_STATE;
            xev.xclient.format       = 32;
            xev.xclient.data.l[0]    = 1;
            xev.xclient.data.l[1]    = Atom_NET_WM_STATE_FULLSCREEN;
            xev.xclient.data.l[2]    = 0;
            
            XSendEvent(THIS_DISPLAY, DefaultRootWindow(THIS_DISPLAY), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
            XFlush(THIS_DISPLAY);

#if 0
            // Here is where we change the resolution.
            //X11::SwitchToBestVideoMode(DefaultScreen(THIS_DISPLAY), width, height);


            // With the resolution now changed, what we want to do now is create a new window, only this one will have override-redirect enabled
            // so that we can display it fullscreen.
            auto newWindow = X11::CreateWindow(this->iwo.vi, this->iwo.colormap, true);

            X11::SetWindowProperty(newWindow, "__fullscreen_window", (void*)newWindow);

            // With the new window created, we need to transfer properties.
            X11::CopyWindowProperties(THIS_WINDOW, newWindow);

            // The name needs to be copied.
            X11::CopyWindowTitle(THIS_WINDOW, newWindow);


            // Now we need to set the internal X11 window to the new one and then delete the old one.
            auto oldWindow = this->iwo.window;
            this->iwo.window = newWindow;

            X11::DestroyWindow(oldWindow);
            XSync(THIS_DISPLAY, True);


            X11::MapWindow(newWindow);
            XSync(THIS_DISPLAY, True);

            XSetInputFocus(THIS_DISPLAY, newWindow, RevertToNone, CurrentTime);
            XSync(THIS_DISPLAY, True);


            // And now the window needs to be sized.
            XMoveResizeWindow(THIS_DISPLAY, THIS_WINDOW, 0, 0, width, height);

            X11::SetWindowProperty(THIS_WINDOW, "__fullscreen_width",  (void*)(size_t)width);
            X11::SetWindowProperty(THIS_WINDOW, "__fullscreen_height", (void*)(size_t)height);


            // We may need to re-hide the cursor.
            if (X11::GetWindowProperty(THIS_WINDOW, "__cursorvisible") == 0)
            {
                XDefineCursor(THIS_DISPLAY, THIS_WINDOW, reinterpret_cast<Cursor>(X11::GetWindowProperty(THIS_WINDOW, "__invisiblecursor")));
                XSync(THIS_DISPLAY, False);
            }
#endif
        }
    }

    void Window::DisableFullscreen()
    {
        if (this->IsFullscreen())
        {
            // We need to set the property at the start because of the way we do event handling. The event
            // handler will look at this property when determining how to handle resizing.
            X11::SetWindowProperty(THIS_WINDOW, "__fullscreen",              (void*)0);
            X11::SetWindowProperty(THIS_WINDOW, "__switching_to_fullscreen", (void*)0);

            // We first want to restore the video mode.
            //X11::RestoreVideoMode(DefaultScreen(THIS_DISPLAY));

            int originalX               =          (int)(size_t)X11::GetWindowProperty(THIS_WINDOW, "__x");
            int originalY               =          (int)(size_t)X11::GetWindowProperty(THIS_WINDOW, "__y");
            unsigned int originalWidth  = (unsigned int)(size_t)X11::GetWindowProperty(THIS_WINDOW, "__width");
            unsigned int originalHeight = (unsigned int)(size_t)X11::GetWindowProperty(THIS_WINDOW, "__height");


            Atom Atom_NET_WM_STATE            = XInternAtom(THIS_DISPLAY, "_NET_WM_STATE",            True);
            Atom Atom_NET_WM_STATE_FULLSCREEN = XInternAtom(THIS_DISPLAY, "_NET_WM_STATE_FULLSCREEN", True);

            XEvent xev;
            memset(&xev, 0, sizeof(xev));
            xev.type                 = ClientMessage;
            xev.xclient.window       = THIS_WINDOW;
            xev.xclient.message_type = Atom_NET_WM_STATE;
            xev.xclient.format       = 32;
            xev.xclient.data.l[0]    = 0;
            xev.xclient.data.l[1]    = Atom_NET_WM_STATE_FULLSCREEN;
            xev.xclient.data.l[2]    = 0;
            
            XSendEvent(THIS_DISPLAY, DefaultRootWindow(THIS_DISPLAY), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
            XMoveResizeWindow(THIS_DISPLAY, THIS_WINDOW, originalX, originalY, originalWidth, originalHeight);
            XFlush(THIS_DISPLAY);

/*
            // Now we go back to a normal, managed window. It's the same process as going fullscreen, only we NOT using override-redirect.
            auto newWindow = X11::CreateWindow(this->iwo.vi, this->iwo.colormap, false);

            // With the new window created, we need to transfer properties.
            X11::CopyWindowProperties(THIS_WINDOW, newWindow);

            // The name needs to be copied.
            X11::CopyWindowTitle(THIS_WINDOW, newWindow);


            // Now we need to set the internal X11 window to the new one and then delete the old one.
            auto oldWindow = this->iwo.window;
            this->iwo.window = newWindow;

            X11::DestroyWindow(oldWindow);
            XSync(THIS_DISPLAY, True);


            X11::MapWindow(newWindow);
            XSync(THIS_DISPLAY, True);

            XSetInputFocus(THIS_DISPLAY, newWindow, RevertToNone, CurrentTime);
            XSync(THIS_DISPLAY, True);


            XMoveResizeWindow(THIS_DISPLAY, THIS_WINDOW, originalX, originalY, originalWidth, originalHeight);


            // We may need to re-hide the cursor.
            if (X11::GetWindowProperty(THIS_WINDOW, "__cursorvisible") == 0)
            {
                XDefineCursor(THIS_DISPLAY, THIS_WINDOW, reinterpret_cast<Cursor>(X11::GetWindowProperty(THIS_WINDOW, "__invisiblecursor")));
                XSync(THIS_DISPLAY, False);
            }
*/
        }
    }

    bool Window::IsFullscreen() const
    {
        return X11::GetWindowProperty(THIS_WINDOW, "__fullscreen") != 0;
    }
}

#endif
