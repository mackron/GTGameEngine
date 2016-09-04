// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Win32
#define GT_Win32

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)

namespace GT
{
    namespace Win32
    {
        /// A helper for creating a default Win32 window.
        HWND CreateHWND();

        /// Deletes a window created with Win32::CreateWindow().
        void DeleteHWND(HWND hWnd);

        /// Sets the cursor of the given window.
        void SetCursor(HWND hWnd, Cursor cursor);

        /// Hides the cursor for the given window.
        void HideCursor(HWND hWnd);
        void HideCursor();

        /// Shows the cursor for the given window.
        void ShowCursor(HWND hWnd);
        void ShowCursor();

        /// Determines whether or not the mouse cursor is over the given window.
        ///
        /// @return True if the cursor is over the given window; false otherwise.
        bool IsCursorOverHWND(HWND hWnd);


        /// Retrieves the window handle of the first window.
        HWND GetFirstHWND();
    }
}

#endif

#endif
