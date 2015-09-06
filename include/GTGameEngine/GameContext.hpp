// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GE_GameContext
#define GT_GE_GameContext

#include "Config.hpp"
#include "WindowManager.hpp"
#include <GTLib/ResultCodes.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/Keyboard.hpp>

#if defined(GT_BUILD_EDITOR)
#include "Editor/Editor.hpp"
#include "GameContextEditorEventHandler.hpp"
#endif

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windef.h>
#endif



namespace GT
{
    class EngineContext;
    class GameState;

    /// Class representing a game context.
    ///
    /// An application can have multiple game context's per engine context.
    class GameContext
    {
    public:

        /// Constructor.
        GameContext(EngineContext &engineContext, GameState &gameState);

        /// Destructor.
        ~GameContext();


        /// Starts up the game context.
        ///
        /// @return A result code specifying whether or not the context was initialized successfully. If the return value is >= 0, it was successful, otherwise it failed.
        bool Startup();

        /// Shuts down the game context.
        void Shutdown();


        /// Retrieves a reference to the engine context.
        ///
        /// @return A reference to the engine context.
        EngineContext & GetEngineContext();

        /// Retrieves a reference to the game state.
        ///
        /// @return A reference to the game state object.
        GameState & GetGameState();

        /// Retrieves a reference to the window manager.
        WindowManager & GetWindowManager();


        /// Runs the game.
        ///
        /// @return The exit code.
        ///
        /// @remarks
        ///     This is where the game loop is run. When this function returns the game will be closed.
        int Run();


        /// Retrieves the number of game windows based on the game state.
        ///
        /// @return The number of game windows.
        ///
        /// @remarks
        ///     This just returns GameState::GetWindowCount().
        unsigned int GetWindowCount();

        /// Retrieves the window at the given index.
        ///
        /// @param index [in] The index of the window to retrieve.
        ///
        /// @return A handle to the window at the given index.
        ///
        /// @remarks
        ///     The window at index 0 should be considered the primary window. The editor will reuse the game windows for itself.
        HWindow GetWindowByIndex(unsigned int index);


        /// Retrieves the size of the given window.
        ///
        /// @param hWindow   [in ] A handle to the window whose size is being retrieved.
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        ///
        /// @return True if the size of the window is retrieved successfully; false otherwise.
        bool GetWindowSize(HWindow hWindow, unsigned int &widthOut, unsigned int &heightOut) const;


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


        /// Shows the given window.
        ///
        /// @param hWindow [in] The window to show.
        void ShowWindow(HWindow hWindow);

        /// Hides the given window.
        ///
        /// @param hWindow [in] The window to hide.
        void HideWindow(HWindow hWindow);


        /// Closes the game.
        ///
        /// @remarks
        ///     This does not immediately shut down the game. Rather, it marks the game as needing to close and waits
        ///     until the end of the current loop iteration.
        void Close();

        /// Steps the game by one frame.
        ///
        /// @remarks
        ///     This will update and render the game.
        void Step();


        /// Pauses the game.
        void Pause();

        /// Resumes the game.
        void Resume();

        /// Determines whether or not the game is paused.
        bool IsPaused() const;


        /// Retrieves a reference to the GUI context.
        GUIContext & GetGUI();


        ///////////////////////////////////////////////////
        // Input

        /// Determines whether or not the given key is currently down.
        ///
        /// @param key [in] The whose down state should be checked.
        ///
        /// @return True if the key is down; false otherwise.
        bool IsKeyDown(GTLib::Key key);

        /// Retrieves the position of the mouse relative to the given window.
        ///
        /// @param hWindow      [in]  The window.
        /// @param mousePosXOut [out] A reference to the variable that will receive the mouse position on the x axis.
        /// @param mousePosYOut [out] A reference to the variable that will receive the mouse position on the y axis.
        bool GetCursorPosition(HWindow hWindow, int &mousePosXOut, int &mousePosYOut) const;

        /// Helper function for determining whether or not the mouse is contained within the given window.
        ///
        /// @param hWindow [in] The window.
        bool IsMouseInsideWindow(HWindow hWindow) const;


        /// Shows the cursor.
        void ShowCursor();

        /// Hides the cursor.
        void HideCursor();

        /// Determines whether or not the cursor is visible.
        bool IsCursorVisible() const;


        ///////////////////////////////////////////////////
        // Editor

        /// Opens the editor.
        ///
        /// @return True if the editor is opened successfully; false otherwise.
        ///
        /// @remarks
        ///     This will call the game state's OnWantToOpenEditor() method. If that returns false the editor will not be opened.
        bool OpenEditor();

        /// Closes the editor.
        void CloseEditor();

        /// Determines whether or not the editor is open.
        ///
        /// @return True if the editor is open; false otherwise.
        bool IsEditorOpen() const;



        ///////////////////////////////////////////////////
        // Inbound Events

        /// Called when the game wants to close.
        void OnWantToClose();

        /// Called after a primary window has been activated.
        ///
        /// @param hWindow [in] The window that was just activated.
        void OnWindowActivated(HWindow hWindow);

        /// Called after a primary window has been deactivated.
        ///
        /// @param hWindow [in] The window that was just deactivated.
        void OnWindowDeactivated(HWindow hWindow);

        /// Called just after a window is created.
        ///
        /// @param hWindow [in] The window that was just created.
        ///
        /// @remarks
        ///     This is always sent to the editor even when it is closed.
        void OnWindowCreated(HWindow hWindow);

        /// Called just before a window is deleted.
        ///
        /// @param hWindow [in] The window that is about to be deleted.
        ///
        /// @remarks
        ///     This is always sent to the editor event when it is closed.
        void OnWindowDeleting(HWindow hWindow);

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

        /// Called when the mouse enters the given window.
        ///
        /// @param hWindow [in] A handle to the window that has just had the mouse enter it.
        void OnMouseEnter(HWindow hWindow);

        /// Called when the mouse leaves the given window.
        ///
        /// @param hWindow [in] A handle to the window that has just had the mouse leave it.
        void OnMouseLeave(HWindow hWindow);

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
        void OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when the mouse is double-clicked on the given window.
        ///
        /// @param hWindow [in] A handle to the window that received the double-click message.
        /// @param button  [in] The index of the button that was double-clicked.
        /// @param xPos    [in] The position of the cursor on the x axis.
        /// @param yPos    [in] The position of the cursor on the y axis.
        void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos);

        /// Called when the mouse wheel is turned.
        void OnMouseWheel(HWindow hWindow, int delta, int xPos, int yPos);

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
        void OnPrintableKeyDown(HWindow hWindow, char32_t character);

        /// Called when a region of the given window needs to be repainted.
        ///
        /// @param hWindow [in] The window that is needing a repaint.
        /// @param rect    [in] The rectangle region that is needing a repaint.
        ///
        /// @remarks
        ///     The rectangle will never be empty.
        ///     @par
        ///     Games will almost never need to handle this function because they will typically repaint the entire windo
        ///     every frame. This is mainly used by the editor because it does not do a full-window update every frame, but
        ///     rather only draws the regions that have actually changed.
        void OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect);


        /// Called when the editor is opened.
        void OnEditorOpened();

        /// Called when the editor is closed.
        void OnEditorClosed();



    private:

        /// A reference to the engine context that owns this game context.
        EngineContext &m_engineContext;

        /// A reference to the game state. This is set in the constructor.
        GameState &m_gameState;


        /// A pointer to the window manager. This should never be null. If a window manager is not specified in the context's constructor a default
        /// one will be created.
        WindowManager* m_pWindowManager;


        /// The GUI context for the game. The editor uses a separate GUI context.
        GUIContext m_gui;


        /// A set of flags for boolean variables.
        uint32_t m_flags;


        /// The time of the last frame. This is used to calculate the delta time between frames.
        double m_lastFrameTime;



#if defined(GT_BUILD_EDITOR)
        /// The editor.
        Editor m_editor;

        /// The editor event handler for detecting when the editor is opened and closed.
        GameContextEditorEventHandler m_editorEventHandler;

        /// Keeps track of whether or not the cursor was visible before opening the editor. We use this to restore the mouse cursor state when the editor is closed.
        bool m_wasCursorVisibleBeforeOpeningEditor;
#endif

    private:

        ///////////////////////////////////////////////////////////////
        // State Flags

        static const uint32_t StateFlag_IsSingleThreadedFlag       = (1U << 0);         ///< Whether or not the game should run in single- or multi-threaded mode.
        static const uint32_t StateFlag_IsOwnerOfWindowManagerFlag = (1U << 1);         ///< Whether or not the context owns the window manager.
        static const uint32_t StateFlag_IsRunningFlag              = (1U << 2);         ///< Whether or not the game is currently running. This is used to track whether or not the game loop should continue looping.
        static const uint32_t StateFlag_IsRunningRealTimeLoopFlag  = (1U << 3);         ///< Whether or not a real-time loop is being used.
        static const uint32_t StateFlag_IsPaused                   = (1U << 4);         ///< Whether or not the game is paused.

        static const uint32_t StateFlag_IsEditorInitialisedFlag    = (1U << 31);        ///< Whether or not the editor has been initialized.


    public:

        ///////////////////////////////////////////////////////////////
        // Error Codes

        static const ResultCode InvalidWindow = (1U << 31) | 1;


    private:    // No copying.
        GameContext(const GameContext &);
        GameContext & operator=(const GameContext &);
    };
}

#endif
