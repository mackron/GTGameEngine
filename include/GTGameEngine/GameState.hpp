// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GameState
#define GT_GameState

#include <GTLib/Serializer.hpp>
#include <GTLib/Deserializer.hpp>
#include <GTLib/Keyboard.hpp>
#include <GTLib/CommandLine.hpp>
#include <GTGameEngine/WindowManager.hpp>

namespace GT
{
    class GameContext;

    /// The base class game states.
    class GameState
    {
    public:

        /// Constructor.
        GameState();

        /// Destructor.
        virtual ~GameState();


        /// Serializes the game state.
        ///
        /// @param serializer [in] A reference to the serializer to write the data to.
        virtual void Serialize(GameContext &gameContext, GTLib::Serializer &serializer);

        /// Deserializes the game state.
        ///
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        virtual void Deserialize(GameContext &gameContext, GTLib::Deserializer &deserializer);


        /// Retrieves the number of windows for the game.
        ///
        /// @remarks
        ///     A game will usually only have a single window, but it can have any number. Indeed, a games doesn't actually need
        ///     a window - a server would not need a window, for example.
        ///     @par
        ///     The first window returned should be considered the primary window.
        virtual unsigned int GetWindowCount(GameContext &gameContext) const;

        /// Retrieves the window at the given index.
        ///
        /// @param index [in] The index of the window to retrieve.
        ///
        /// @return A handle to the window at the given index.
        ///
        /// @remarks
        ///     The window at index 0 should be considered the primary window. The editor will reuse the game windows for itself.
        virtual HWindow GetWindowByIndex(GameContext &gameContext, unsigned int index);


        /// Steps the game.
        ///
        /// @param deltaTimeInSeconds [in] The time in seconds between frames.
        ///
        /// @remarks
        ///     There are two parts to stepping the game - updating and rendering. It is up to the game state to decide how it
        ///     wants to do the update.
        virtual void Step(GameContext &gameContext, double deltaTimeInSeconds);


        /////////////////////////////////
        // Events

        /// Called once, at the very start of the running of the game.
        ///
        /// @param cmdLine [in] The command line that was passed to the process from the operation system.
        ///
        /// @return True if the game state was initialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This is called at the beginning of GameContext::Run().
        ///     @par
        ///     The game will not continue running if this returns false.
        virtual bool OnStarting(GameContext &gameContext, const GTLib::CommandLine &cmdLine);

        /// Called once, at the very end of the game loop.
        ///
        /// @remarks
        ///     This is called outside of the loop, and just before the game returns from GameContext::Run().
        virtual void OnClosing(GameContext &gameContext);


        /// Called when the game is paused.
        virtual void OnPause(GameContext &gameContext);

        /// Called when the game is resumed from a pause state.
        virtual void OnResume(GameContext &gameContext);


        /// Called when the game wants to close.
        ///
        /// @return True if the game should continue to close, or false if the game should remain open.
        ///
        /// @remarks
        ///     The default implementation will return true.
        virtual bool OnWantToClose(GameContext &gameContext);

        /// Called just after a window is created.
        ///
        /// @param hWindow [in] The window that was just created.
        virtual void OnWindowCreated(GameContext &gameContext, HWindow hWindow);

        /// Called just before a window is deleted.
        ///
        /// @param hWindow [in] The window that is about to be deleted.
        virtual void OnWindowDeleting(GameContext &gameContext, HWindow hWindow);

        /// Called after a primary window has been activated.
        ///
        /// @param hWindow [in] The window that was just activated.
        virtual void OnWindowActivated(GameContext &gameContext, HWindow hWindow);

        /// Called after a primary window has been deactivated.
        ///
        /// @param hWindow [in] The window that was just deactivated.
        virtual void OnWindowDeactivated(GameContext &gameContext, HWindow hWindow);

        /// Called when the given window has been resized.
        ///
        /// @param hWindow [in] A handle to the window that has been resized.
        /// @param width   [in] The new width of the window.
        /// @param height  [in] The new height of the window.
        virtual void OnWindowResized(GameContext &gameContext, HWindow hWindow, unsigned int width, unsigned int height);

        /// Called when the given window has been moved.
        ///
        /// @param hWindow [in] A handle to the window that moved.
        /// @param xPos    [in] The new position of the window on the x axis.
        /// @param yPos    [in] The new position of the window on the y axis.
        ///
        /// @remarks
        ///     If the window is a child the position will be relative to the parent window.
        virtual void OnWindowMoved(GameContext &gameContext, HWindow hWindow, int xPos, int yPos);

        /// Called when the mouse is moved over the given window.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse move over it.
        /// @param xPos    [in] The position of the cursor relative to the top left corner of the window.
        /// @param yPos    [in] The position of the cursor relative to the top left corner of the window.
        virtual void OnMouseMove(GameContext &gameContext, HWindow hWindow, int xPos, int yPos);

        /// Called when the mouse enters the given window.
        ///
        /// @param hWindow [in] A handle to the window that has just had the mouse enter it.
        virtual void OnMouseEnter(GameContext &gameContext, HWindow hWindow);

        /// Called when the mouse leaves the given window.
        ///
        /// @param hWindow [in] A handle to the window that has just had the mouse leave it.
        virtual void OnMouseLeave(GameContext &gameContext, HWindow hWindow);

        /// Called when a mouse button is pressed on the given window.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse button clicked on it.
        /// @param button  [in] The index of the mouse button that was pressed.
        /// @param xPos    [in] The position of the mouse cursor on the x axis.
        /// @param yPos    [in] The position of the mouse cursor on the y axis.
        virtual void OnMouseButtonPressed(GameContext &gameContext, HWindow hWindow, int button, int xPos, int yPos);

        /// Called when a mouse button is released.
        ///
        /// @param hWindow [in] A handle to the window that had the mouse button released.
        /// @param button  [in] The index of the mouse button that was pressed.
        /// @param xPos    [in] The position of the mouse cursor on the x axis.
        /// @param yPos    [in] The position of the mouse cursor on the y axis.
        virtual void OnMouseButtonReleased(GameContext &gameContext, HWindow hWindow, int button, int xPos, int yPos);

        /// Called when the mouse is double-clicked on the given window.
        ///
        /// @param hWindow [in] A handle to the window that received the double-click message.
        /// @param button  [in] The index of the button that was double-clicked.
        /// @param xPos    [in] The position of the cursor on the x axis.
        /// @param yPos    [in] The position of the cursor on the y axis.
        virtual void OnMouseButtonDoubleClicked(GameContext &gameContext, HWindow hWindow, int button, int xPos, int yPos);

        /// Called when the mouse wheen is turned.
        virtual void OnMouseWheel(GameContext &gameContext, HWindow hWindow, int delta, int xPos, int yPos);

        /// Called when a key is pressed.
        ///
        /// @param hWindow [in] A handle to the window that has the keyboard focus and thus received the key press event.
        /// @param key     [in] The virtual key code for the key that was pressed.
        virtual void OnKeyPressed(GameContext &gameContext, HWindow hWindow, GTLib::Key key);
        virtual void OnKeyPressedAutoRepeat(GameContext &gameContext, HWindow hWindow, GTLib::Key key);

        /// Called when a key is released.
        ///
        /// @param hWindow [in] A handle to the window that has the keyboard focus and thus received the key release event.
        /// @param key     [in] The virtual key code for the key that was released.
        virtual void OnKeyReleased(GameContext &gameContext, HWindow hWindow, GTLib::Key key);

        /// Called when a printable key is pressed, and auto-repeated.
        ///
        /// @param hWindow   [in] A handle to the window that has the keyboard focus and thus received the key event.
        /// @param character [in] The character encoded as UTF-32.
        ///
        /// @remarks
        ///     When text operations are required (such as typing in a text box), us this event over OnKeyPressed(). This
        ///     difference is that this passes the unicode representation of the character and is also auto-repeated.
        virtual void OnPrintableKeyDown(GameContext &gameContext, HWindow hWindow, char32_t character);

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
        virtual void OnPaintWindow(GameContext &gameContext, HWindow hWindow, const GTLib::Rect<int> &rect);

        /// Called when a timer is triggered.
        virtual void OnTimer(GameContext &gameContext, HWindow hWindow, size_t timerID);



        /// Called when the editor is wanting to open.
        ///
        /// @return True if the editor should be opened; false if the editor should not be opened.
        ///
        /// @return The default implementation of this method always returns true.
        virtual bool OnWantToOpenEditor(GameContext &gameContext);

        /// Called after the editor has opened.
        ///
        /// @remarks
        ///     This is called after OnWantToOpenEditor(), and only if that returns true.
        virtual void OnEditorOpened(GameContext &gameContext);

        /// Called after the editor has closed.
        virtual void OnEditorClosed(GameContext &gameContext);
    };
}

#endif
