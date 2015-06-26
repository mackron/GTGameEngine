// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_EditorEventHandler_hpp_
#define __GT_EditorEventHandler_hpp_

#include "../WindowManager.hpp"

namespace GT
{
    /// Base class for handling events from the editor.
    class EditorEventHandler
    {
    public:

        /// Constructor.
        EditorEventHandler();

        /// Destructor.
        virtual ~EditorEventHandler();


        /// Called after the editor has been opened.
        virtual void OnEditorOpened();

        /// Called after the editor has been closed.
        virtual void OnEditorClosed();


        /// Called when the main game window is activated.
        ///
        /// @param hWindow [in] The window that was just activated.
        virtual void OnWindowActivated(HWindow hWindow);

        /// Called when the main game window is deactivated.
        ///
        /// @param hWindow [in] The window that was just deactivated.
        virtual void OnWindowDeactivated(HWindow hWindow);

        /// Called when the mouse is moved while over the top of the given window.
        ///
        /// @param hWindow [in] A handle to the window.
        /// @param xPos    [in] The position of the cursor on the x axis relative to the top-left of the window's client area.
        /// @param xPos    [in] The position of the cursor on the y axis relative to the top-left of the window's client area.
        ///
        /// @remarks
        ///     The position is relative to the client area of the window.
        virtual void OnMouseMove(HWindow hWindow, int xPos, int yPos);

        /// Called when a mouse button is pressed on the given window.
        ///
        /// @param hWindow [in] A handle to the window.
        /// @param button  [in] The button.
        /// @param xPos    [in] The position of the cursor on the x axis relative to the top-left of the window's client area.
        /// @param xPos    [in] The position of the cursor on the y axis relative to the top-left of the window's client area.
        ///
        /// @remarks
        ///     The position is relative to the client area of the window.
        virtual void OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when a mouse button is released on the given window.
        ///
        /// @param hWindow [in] A handle to the window.
        /// @param button  [in] The button.
        /// @param xPos    [in] The position of the cursor on the x axis relative to the top-left of the window's client area.
        /// @param xPos    [in] The position of the cursor on the y axis relative to the top-left of the window's client area.
        ///
        /// @remarks
        ///     The position is relative to the client area of the window.
        virtual void OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when a mouse button is double-clicked on the given window.
        ///
        /// @param hWindow [in] A handle to the window.
        /// @param button  [in] The button.
        /// @param xPos    [in] The position of the cursor on the x axis relative to the top-left of the window's client area.
        /// @param xPos    [in] The position of the cursor on the y axis relative to the top-left of the window's client area.
        ///
        /// @remarks
        ///     The position is relative to the client area of the window.
        virtual void OnMouseButtonDoubleClick(HWindow hWindow, int button, int xPos, int yPos);
    };
}

#endif
