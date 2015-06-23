// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_WindowManager_DefaultWin32_hpp_
#define __GT_WindowManager_DefaultWin32_hpp_

#include "WindowManager_Win32.hpp"

#if defined(GT_PLATFORM_WINDOWS)


namespace GT
{
    class GameContext;

    

    /// Default Windows implementation of the window manager.
    class WindowManager_DefaultWin32 : public WindowManager_Win32
    {
    public:

        /// Constructor.
        WindowManager_DefaultWin32(GameContext* pGameContext);

        /// Destructor.
        ~WindowManager_DefaultWin32();


        /// Retrieves a pointer to the game context.
        GameContext* GetGameContext() { return m_pGameContext; }




        /////////////////////////////////////////////
        // To Be Implemented by Child Classes

        /// @copydoc WindowManager_Win32::OnClose()
        virtual void OnClose();

        /// @copydoc WindowManager_Win32::OnMove()
        virtual void OnMove(HWindow hWindow, int xPos, int yPos);

        /// @copydoc WindowManager_Win32::OnSize()
        virtual void OnSize(HWindow hWindow, unsigned int width, unsigned int height);

        /// @copydoc WindowManager_Win32::OnMouseEnter()
        virtual void OnMouseEnter(HWindow hWindow);

        /// @copydoc WindowManager_Win32::OnMouseLeave()
        virtual void OnMouseLeave(HWindow hWindow);

        /// @copydoc WindowManager_Win32::OnMouseMove()
        virtual void OnMouseMove(HWindow hWindow, int mousePosX, int mousePosY);

        /// @copydoc WindowManager_Win32::OnMouseButtonPressed()
        virtual void OnMouseButtonPressed(HWindow hWinodw, int button, int mousePosX, int mousePosY);
        
        /// @copydoc WindowManager_Win32::OnMouseButtonReleased()
        virtual void OnMouseButtonReleased(HWindow hWindow, int button, int mousePosX, int mousePosY);

        /// @copydoc WindowManager_Win32::OnMouseButtonDoubleClicked()
        virtual void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int mousePosX, int mousePosY);

        /// @copydoc WindowManager_Win32::OnKeyPressed()
        virtual void OnKeyPressed(HWindow hWindow, GTLib::Key key);

        /// @copydoc WindowManager_Win32::OnKeyReleased()
        virtual void OnKeyReleased(HWindow hWindow, GTLib::Key key);

        /// @copydoc WindowManager_Win32::OnPrintableKeyDown()
        virtual void OnPrintableKeyDown(HWindow hWindow, char32_t character);

        /// @copydoc WindowManager_Win32::OnPaintWindow()
        virtual void OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect);
        


    private:

        /// A pointer to the game context that will be associated with the window. This is required so that events can be posted to it.
        GameContext* m_pGameContext;
    };
}

#endif

#endif
