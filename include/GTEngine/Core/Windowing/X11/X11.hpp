// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

/**
*   \file   X11/X11.hpp
*
*   This file contains X11-specific functionality.
*
*   All of the normal X11 headers are included from this file just to make things a bit easier.
*
*   The X11 section of GTLib (this file) introduces a few concepts that aren't part of the normal X11 API. The
*   biggest thing is the use of Win32-style event handlers (window procedures in Win32). These allow us to more
*   easily handle window events in a cross-platform manner at higher levels of the GTLib API.
*
*   GTLib only supports a single display in X11. It is set when the X11 component is initialised with X11::Initialize().
*   On X11 systems, X11::Initialize() must always be called before creating windows.
*
*   GTLib does not use the notion of a Display in X11. Instead, it is assumed that all windows share the same display.
*   A global display object is created that is shared across all windowing operations.
*
*   Since setting/getting custom window properties is completely ridiculous with the normal X11 API, we've introduced
*   a simpler way of doing this. Just use SetWindowProperty() and GetWindowProperty(). Read the documentation on
*   these functions to get an idea how they work - but don't worry, they're much, much better than the normal X11 way...
*
*   \section usage Usage
*
*   The X11 namespace uses metadata to attach additional details to an X11 Display/Window combo. It needs to be aware
*   of any windows that are currently in use. The OnWindowCreated() and OnWindowDestroyed() functions need to be used
*   for any window that needs to be managed by the GTGUI::X11 namespace. Without doing this, other functions won't
*   work.
*/

#ifndef GT_X11
#define GT_X11

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include "../WindowEventHandler.hpp"
#include "../../Cursors.hpp"
#include "../../Vector.hpp"

namespace GT
{
    namespace X11
    {
        /**
        *   \brief               Initialises the X11 sub-system.
        *   \param  display [in] The display to use with all windows.
        *
        *   \remarks
        *       If \c display is null, a default display will be created. It can be retrieved with X11::GetDisplay().
        */
        void Initialize(Display* display);

        /**
        *   \brief  Uninitializes the X11 sub-system. This does NOT close the display.
        */
        void Uninitialize();



        /**
        *   \brief  Retrieves the display object.
        *   \return The default display object.
        *
        *   \remarks
        *       If the default display is not yet been opened, it will be opened and returned.
        */
        //Display* GetDisplay();

        /**
        *   \brief              When a window is created, this function should be called to let GTLib know that there is a new window that needs to be managed.
        *   \param  window [in] The window handle that was just created.
        *
        *   \remarks
        *       Do not call this function in response to the CreateNotify event. Call it straight after creating the window with XCreateWindow().
        *       \par
        *       This function will set the WM_DELETE_WINDOW protocol on the window so that the OnClose() event can be handled correctly. Be aware of this
        *       if you are doing completely manual window creation and event handling.
        */
        void OnCreateWindow(::Window window);

        /**
        *   \brief               The opposite of OnWindowCreated(). This function should be called when a window is destroyed.
        *   \param  display [in] The connection to the X server.
        *   \param  window  [in] The window that was just destroyed.
        *
        *   \remarks
        *       This function will be called automatically from the default event handler.
        */
        void OnDestroyWindow(::Window window);


        /**
        *   \brief                 Helper function for creating a pretty standard window suitable for GTGUI.
        *   \param  display   [in] The connection to the X server.
        *   \param  vi        [in] A pointer to the XVisualInfo structure contining information about the visual to use with the window.
        *   \param  colourmap [in] The colour map the window should use.
        *   \return                The X11 Window handle to the new window.
        *
        *   \remarks
        *       The returned window is NOT a GTGUI::Window, but an Xlib window handle.
        *       \par
        *       This function will call OnWindowCreated() internally. No need to call it again.
        *       \par
        *       The new window will not be made visible.
        */
        ::Window CreateWindow(XVisualInfo *vi, Colormap colourmap, bool overrideRedirect = false);

        /**
        *   \brief               Destroys a window created with GTGUI::X11::CreateWindow().
        *   \param  display [in] The connection to the X server.
        *   \parma  window  [in] A handle to the window that is being destroyed.
        */
        void DestroyWindow(::Window window);


        /**
        *   \brief                     Sets the value of a custom property of a window.
        *   \param  display       [in] The connection to the X server.
        *   \param  window        [in] The window whose having it's property set.
        *   \param  propertyName  [in] The name of the property that's being set.
        *   \param  propertyValue [in] The value of the property. This is just a pointer.
        *
        *   \remarks
        *       The value of the property is just a pointer. It can point to anything you like. It is the responsibility of the application
        *       to allocate and free the data the pointer points to.
        */
        void SetWindowProperty(::Window window, const char *propertyName, void *propertyValue);

        /**
        *   \brief                    Retrieves the value of a custom window property.
        *   \param  display      [in] The connection to the X server.
        *   \param  window       [in] The window whose property is being retrieved.
        *   \param  propertyName [in] the name of the property to retrieve.
        *   \return                   The value of the property, as set by SetWindowProperty(). Returns nullptr
        */
        void* GetWindowProperty(::Window window, const char *propertyName);

        /// Copies the properties of one window to another.
        ///
        /// @param source [in] The window whose properties are being copied.
        /// @param dest   [in] The destination window.
        void CopyWindowProperties(::Window source, ::Window dest);


        /// Copies the title of a window onto another.
        ///
        /// @param source [in] The window whose title is being copied.
        /// @param dest   [in] The window whose title is being set.
        void CopyWindowTitle(::Window source, ::Window dest);



        /**
        *   \brief  Sets the title of the given window.
        */


        /**
        *   \brief  Checks if the given event is an on-close event (when the X button is pressed).
        */
        bool IsOnCloseEvent(XEvent &e);


        /**
        *   \brief  Retrieves the event handler used by the given window.
        */
        WindowEventHandler & GetWindowEventHandler(::Window window);

        /**
        *   \brief  Sets the event handler to use with the window.
        */
        void SetWindowEventHandler(::Window window, WindowEventHandler &eventHandler);

        /**
        *   \brief         The default X11 event handler.
        *   \param  e [in] The event that needs to be handled.
        */
        void DefaultEventHandlerProc(XEvent &e);


        /// Retrieves an index to the best video mode for the given resolution.
        ///
        /// @param screen [in]      The screen to use for retrieving the video mode. Typically the screen from DefaultScreen(display).
        /// @param width  [in, out] The width.
        /// @param height [in, out] The height.
        ///
        /// @return An index that can be used with the returned array from XF86VidModeGetAllModeLines(). Returns -1 if anything goes wrong.
        int FindBestVideoMode(int screen, unsigned int &width, unsigned int &height);

        /// Switches to the given video mode retrieved with FindBestVideoMode().
        ///
        /// @param screen [in] The screen. Typically the screen from DefaultScreen(display).
        /// @param mode   [in] The mode to switch to, retrieved from FindBestVideoMode().
        void SwitchVideoMode(int screen, int mode);

        /// Helper function for switching to the best video mode for the given resolution.
        ///
        /// @param screen [in]      The screen. Typically the screen from DefaultScreen(display).
        /// @param width  [in, out] The width. This will receive the chosen width.
        /// @param height [in, out] The height. This will receive the chosen height.
        void SwitchToBestVideoMode(int screen, unsigned int &width, unsigned int &height);

        /// Restores the default video mode for the given screen.
        ///
        /// @param screen [in] The screen whose default video mode is being restored.
        void RestoreVideoMode(int screen);


        /// Maps a window, only returning when the MapNotify event is processed.
        ///
        /// @param window [in] The window to map.
        void MapWindow(::Window window);
        
        /// Maps a window, minimizes it, and only returns when the MapNotify event is processed.
        ///
        /// @param window [in] The window to map and minimize.
        void MapWindowMinimized(::Window window);
        
        /// Maps a window, maximizes it, and only returns when the MapNotify event is processed.
        ///
        /// @param window [in] The window to map and maximize.
        void MapWindowMaximized(::Window window);

        /// Unmaps the given window.
        ///
        /// @param window [in] The window to unmap.
        void UnmapWindow(::Window window);


        /// Retrieves a ::Window handle to the oldest window.
        ::Window GetFirstWindow();


        /// Sets the cursor of the given window.
        ///
        /// @param window [in] The window whose window is having it's window changed.
        /// @param cursor [in] The new cursor to set on the window.
        void SetCursor(::Window window, Cursor cursor);

        /// Retrieves the X11 cursor from the given GTLib cursor.
        ///
        /// @param cursor [in] The SystemCursor whose X11 counterpart is being retrieved.
        ::Cursor GetX11Cursor(Cursor cursor);



        /////////////////////////////////////////////
        // Clipboard.

        /// Sets the text of the clipboard target.
        ///
        /// @param text     [in] The text to place onto the clipboard.
        /// @param sizeInTs [in] The length of the text.
        /// @param target   [in] The target to place the text. See remarks.
        ///
        /// @remarks
        ///     Valid options for "target" are these atoms:
        ///       - "PRIMARY"
        ///       - "CLIPBOARD"
        void SetClipboardText(const char* text, Atom target);
        void SetClipboardText(const char* text, ptrdiff_t sizeInTs, Atom target);

        /// Retrieves the text of the given clipboard target.
        ///
        /// @param target [in] The target to retrieve the text from.
        const char* GetClipboardText(Atom target);

        /// Retrieves the length of the text of the given clipboard target.
        ///
        /// @param target [in] The target to retrieve the text length from.
        size_t GetClipboardTextLength(Atom target);

        /// Retrieves the "TARGETS" atom for clipboard operations.
        Atom GetAtomTARGETS();

        /// Retrieves the "PRIMARY" atom for clipboard operations.
        Atom GetAtomPRIMARY();

        /// Retrieves the "CLIPBOARD" atom for clipboard operations.
        Atom GetAtomCLIPBOARD();

        /// Retrieves the "TEXT" atom for clipboard operations.
        Atom GetAtomTEXT();

        /// Handles the SelectionRequest event.
        void HandleEvent_SelectionRequest(const XEvent &e);



        /////////////////////////////////////////////
        // Properties.

        /// Structure representing a property attached to a window.
        struct Property
        {
            Property()
                : type(), format(), itemCount(), data()
            {
            }

            Atom                          type;
            int                           format;       // 8, 16, or 32.
            unsigned long                 itemCount;
            Vector<unsigned char> data;
        };

        /// Retrieves a property from the given window.
        ///
        /// @param window   [in ] The window whose property is being retrieved.
        /// @param property [in ] The atom representing the property to retrieve.
        /// @param valueOut [out] A reference to the Property structure that will receive the property.
        void GetWindowProperty(::Window window, Atom property, Property &valueOut);
    }
}

#endif  // __linux__

#endif
