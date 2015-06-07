// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GameContext_hpp_
#define __GT_GE_GameContext_hpp_

#include "Config.hpp"
#include "WindowManager.hpp"
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/Keyboard.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif

namespace GT
{
    class EngineContext;
    class GameDisplay;
    class GameDisplay_Windowed;
    class GraphicsInterface;


    static const int MouseButton_Left   = 1;
    static const int MouseButton_Right  = 2;
    static const int MouseButton_Middle = 3;



    /// Class representing a game context.
    ///
    /// An application can have multiple game context's per engine context.
    class GameContext
    {
    public:

        /// Constructor.
        GameContext(EngineContext &engineContext);

        /// Destructor.
        ~GameContext();


        /// Starts up the game context.
        ///
        /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
        ResultCode Startup();

        /// Shuts down the game context.
        void Shutdown();


        /// Creates a game window.
        ///
        /// @param type   [in] The type of window to create.
        /// @param title  [in] The title of the window.
        /// @param xPos   [in] The position of the window on the x axis.
        /// @param yPos   [in] The position of the window on the y axis.
        /// @param width  [in] The width of the window.
        /// @param height [in] The height of the winodw.
        HWindow CreateMainWindow(WindowType type, const char* title, int xPos, int yPos, unsigned int width, unsigned int height);

        /// Creates a child window.
        ///
        /// @param hParent [in] A handle to the parent window.
        /// @param xPos    [in] The position of the window on the x axis, relative to the parent.
        /// @param yPos    [in] The position of the window on the y axis, relative to the parent.
        /// @param width   [in] The width of the window.
        /// @param height  [in] The height of the window.
        HWindow CreateChildWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height);

        /// Creates a popup window.
        ///
        /// @param hParent [in] A handle to the parent window.
        /// @param xPos    [in] The position of the window on the x axis, relative to the parent.
        /// @param yPos    [in] The position of the window on the y axis, relative to the parent.
        /// @param width   [in] The width of the window.
        /// @param height  [in] The height of the window.
        ///
        /// @remarks
        ///     A popup window is for things like dropdown menus and tooltips.
        HWindow CreatePopupWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height);

        /// Destroys the given window.
        ///
        /// @param hWindow [in] A handle to the window to destroy.
        void DeleteWindow(HWindow hWindow);


#if defined(GT_PLATFORM_WINDOWS)
        /// Creates a windowed display from the given window.
        ///
        /// @param renderingDevice [in] The rendering device that will be used for rendering to the display.
        /// @param hWnd            [in] The window to create the display from.
        ///
        /// @return A pointer to the new display, or null if an error occurred.
        GameDisplay* CreateWindowedDisplay(GraphicsInterface &graphicsInterface, HWND hWnd);

        /// Retrieves a windowed game display from the given window.
        ///
        /// @param hWnd [in] A handle to the window whose game display object is being retrieved.
        ///
        /// @return A pointer to the game display, or null if it cannot be found.
        GameDisplay* GetWindowedDisplayFromWindow(HWND hWnd);
#endif

        /// Deletes the given display.
        ///
        /// @param display [in] A pointer to the display to delete.
        void DeleteDisplay(GameDisplay* display);



        /// Steps the game by one frame.
        ///
        /// @remarks
        ///     This will update and render the game.
        void Step();



        ///////////////////////////////////////////////////
        // Inbound Events

        /// Called when the game wants to close.
        void OnWantToClose();

        /// Called when the given window has been resized.
        ///
        /// @param hWindow [in] A handle to the window that has been resized.
        /// @param width   [in] The new width of the window.
        /// @param height  [in] The new height of the window.
        void OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height);

        /// Called when the given window has been moved.
        ///
        /// @param hWindow [in] A handle to the window that moved.
        /// @param xPos    [in] The new position of the window on the x axis.
        /// @param yPos    [in] The new position of the window on the y axis.
        ///
        /// @remarks
        ///     If the window is a child the position will be relative to the parent window.
        void OnWindowMoved(HWindow hWindow, int xPos, int yPos);

        /// Called when the mouse is moved over the given window.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse move over it.
        /// @param xPos    [in] The position of the cursor relative to the top left corner of the window.
        /// @param yPos    [in] The position of the cursor relative to the top left corner of the window.
        void OnMouseMove(HWindow hWindow, int xPos, int yPos);

        /// Called when a mouse button is pressed on the given window.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse button clicked on it.
        /// @param button  [in] The index of the mouse button that was pressed.
        /// @param xPos    [in] The position of the mouse cursor on the x axis.
        /// @param yPos    [in] The position of the mouse cursor on the y axis.
        void OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when a mouse button is released.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse button released.
        /// @param button  [in] The index of the mouse button that was pressed.
        /// @param xPos    [in] The position of the mouse cursor on the x axis.
        /// @param yPos    [in] The position of the mouse cursor on the y axis.
        void OnMouseButtonReleased(HWindow hWinodw, int button, int xPos, int yPos);

        /// Called when the mouse is double-clicked on the given window.
        ///
        /// @param hWindow [in] A handle to the window that received the double-click message.
        /// @param button  [in] The index of the button that was double-clicked.
        /// @param xPos    [in] The position of the cursor on the x axis.
        /// @param yPos    [in] The position of the cursor on the y axis.
        void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when a key is pressed.
        ///
        /// @param hWindow [in] A handle to the window that has the keyboard focus and thus received the key press event.
        /// @param key     [in] The virtual key code for the key that was pressed.
        void OnKeyPressed(HWindow hWindow, GTLib::Key key);

        /// Called when a key is released.
        ///
        /// @param hWindow [in] A handle to the window that has the keyboard focus and thus received the key release event.
        /// @param key     [in] The virtual key code for the key that was released.
        void OnKeyReleased(HWindow hWindow, GTLib::Key key);

        /// Called when a printable key is pressed, and auto-repeated.
        ///
        /// @param hWindow   [in] A handle to the window that has the keyboard focus and thus received the key event.
        /// @param character [in] The character encoded as UTF-32.
        ///
        /// @remarks
        ///     When text operations are required (such as typing in a text box), us this event over OnKeyPressed(). This
        ///     difference is that this passes the unicode representation of the character and is also auto-repeated.
        void OnPrintableKeyDown(HWindow hWindow, GTLib::Key key);




    private:

        /// A reference to the engine context that owns this game context.
        EngineContext &m_engineContext;

        /// The list of window displays.
        GTLib::Vector<GameDisplay_Windowed*> m_windowedDisplays;


        /// A set of flags for boolean variables.
        uint32_t m_flags;


    private:

        ///////////////////////////////////////////////////////////////
        // State Flags

        static const uint32_t IsSingleThreaded = 0x00000001;        //< Whether or not the game should run in single- or multi-threaded mode.



    public:

        ///////////////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode InvalidWindow = (1 << 31) | 1;


    private:    // No copying.
        GameContext(const GameContext &);
        GameContext & operator=(const GameContext &);
    };
}

#endif
