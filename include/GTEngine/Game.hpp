
#ifndef __GTEngine_Game_hpp_
#define __GTEngine_Game_hpp_

#include "GameEventQueue.hpp"
#include "GameWindowEventHandler.hpp"
#include "Scene.hpp"
#include "UserConfig.hpp"
#include "Rendering/Renderer.hpp"
#include <GTCore/Threading.hpp>
#include <GTCore/Timing.hpp>
#include <GTType/FontServer.hpp>

namespace GTEngine
{
    static const size_t MouseBufferSize   = 3;
    static const float  MouseSmoothFactor = 0.5f;

    /**
    *   \brief  Base class for the class representing a game.
    *
    *   This class needs to be inheritted, with the event methods implemented by the child class.
    *
    *   \section    Mouse
    *   The game's mouse can be in one of two states: captured or uncaptured. By default, the mouse is uncaptured. The way a mouse works is different
    *   depending on whether or not it is captured.
    *
    *   A captured mouse is hidden and will always be placed in the center of the window. It's movements are defined as offsets from the center. A
    *   captured mouse will have smoothing applied to it, making it useful for things like first-person cameras. MouseMove events are ignored when
    *   the mouse is captured. You retrieved smoothed offsets using Game::GetSmoothedMouseOffset(). Use Game::CaptureMouse() to capture the mouse
    *   and Game::ReleaseMouse() to use uncaptured mode.
    *   \endsection
    */
    class Game
    {
    public:

        /**
        *   \brief  Constructor.
        *
        *   \remarks
        *       The constructor is where all initialisation takes place. Use operator bool or IsInitialised() to check if initialisation was successful.
        *       \par
        *       If initialisation was not successful, Run() will return immediately. Use Run() to start running the game.
        */
        Game(int argc, char **argv);

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       This is where everything will be uninitialised.
        */
        virtual ~Game();

        /**
        *   \brief  Runs the game.
        *   \return An exit code. Zero if the game was run and closed as expected. Non-zero if an error occurs.
        *
        *   \remarks
        *       This method will do everything including initialisation/uninitialisation.
        */
        int Run();

        /**
        *   \brief  Determines whether or not the game is initialised.
        *   \return True if the game was initialised successfully; false otherwise.
        */
        virtual bool IsInitialised() const { return this->isInitialised; }

        /**
        *   \brief  Marks the game for closure.
        *
        *   \remarks
        *       This does not immediately shutdown the game. Instead it marks the game as wanting to close. It will return before
        *       the game has actually closed.
        */
        void Close();

        /**
        *   \brief  Sends an event to the game.
        *
        *   \remarks
        *       This function is thread-safe.
        */
        void SendEvent(const GameEvent &e);


        /**
        *   \brief  Retrieves a reference to the internal GUI server.
        */
        GTGUI::Server & GetGUI();

        /**
        *   \brief  Steps the GUI by the given delta.
        */
        void StepGUI();
        void StepGUI(double deltaTimeInSeconds);


        /**
        *   \brief  Retrieves a pointer to the main game window.
        */
        GTCore::Window * GetWindow() { return this->window; }

        /**
        *   \brief  Retrieves the current position of the mouse. This is relative to the main game window.
        */
        void GetMousePosition(int &x, int &y);

        /**
        *   \brief  Retrieves the smoothed position of the mouse. This is required when the mouse is captured.
        */
        void GetSmoothedMouseOffset(float &x, float &y);

        /**
        *   \brief  Sets the position of the mouse relative to the game's main window.
        */
        void SetMousePosition(int x, int y);

        /**
        *   \brief  Retrieves the time in seconds between the last two frames. Allows time based movement.
        */
        double GetDeltaTimeInSeconds() const { return this->deltaTimeInSeconds; }

        /**
        *   \brief  Retrieves a pointer to the games font cache.
        */
        GTType::FontServer * GetFontServer() { return this->fontServer; }

        /**
        *   \brief  Retrieves a pointer to the default font.
        */
        GTType::Font * GetDefaultFont() { return this->defaultFont; }

        /**
        *   \brief  Captures the mouse.
        */
        void CaptureMouse();

        /**
        *   \brief  Releases the mouse.
        */
        void ReleaseMouse();


        /// Determines if a key is currently pressed.
        /// @param key [in] The key code to check.
        /// @return True if the key is current held down.
        bool IsKeyDown(GTCore::Key key) const;


        /**
        *   \brief  Pauses the game.
        *
        *   \remarks
        *       When the game is paused, scene nodes will continue to render, but nothing will be stepped.
        *       \par
        *       This will also pause the current scene. An application should never need to explicately pause/resume a scene.
        */
        void Pause();

        /**
        *   \brief  Resumes the paused game.
        *
        *   \remarks
        *       This will also resume the current scene. An application should never need to explicately pause/resume a scene.
        */
        void Resume();

        /**
        *   \brief  Determines whether or not the game is paused.
        */
        bool IsPaused() const { return this->paused; }


    // Operators.
    public:

        /**
        *   \brief  Will return true if the game is initialised and valid. False otherwise.
        */
        operator bool() const { return this->IsInitialised(); }


    protected:

        void CacheMousePosition();



        
    protected:  // Event handlers.

        /**
        *   \brief  Called when the game is updating. This is called from the update thread, just before the scene is updated.
        */
        virtual void OnUpdate();

        /**
        *   \brief  Called on the rendering thread just before the main scene is drawn.
        */
        virtual void OnDraw();

        /**
        *   \brief  Called on the rendering thread after the main rendering commands have been drawn, and just before swapping the buffers.
        *
        *   \remarks
        *       This is a good time to render a GUI with Renderer::DrawGUI().
        */
        virtual void OnPostDraw();

        /**
        *   \brief  Called just before the next frame has started.
        *
        *   \remarks
        *       This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        *       executed on the main thread.
        */
        virtual void OnStartFrame();

        /**
        *   \brief  Called after the current frame has finished.
        *
        *   \remarks
        *       This function is completely synchronised. Neither the update nor the rendering thread is running at this point. It will be
        *       executed on the main thread.
        */
        virtual void OnEndFrame();


        // All of the events below are called on the update thread. It is safe to do draw calls on the renderer's back draw-call cache.
        virtual void OnSize(unsigned int width, unsigned int height);
        virtual void OnMouseMove(int x, int y);
        virtual void OnMouseWheel(int delta, int x, int y);
        virtual void OnMouseButtonDown(GTCore::MouseButton button, int x, int y);
        virtual void OnMouseButtonUp(GTCore::MouseButton button, int x, int y);
        virtual void OnMouseButtonDoubleClick(GTCore::MouseButton button, int x, int y);
        virtual void OnKeyPressed(GTCore::Key key);
        virtual void OnKeyReleased(GTCore::Key key);
        virtual void OnKeyDown(GTCore::Key key);
        virtual void OnKeyUp(GTCore::Key key);
        virtual void OnReceiveFocus();
        virtual void OnLoseFocus();


        /**
        *   \brief  Called when any rendering command buffers need to be swapped.
        */
        virtual void OnSwapRCBuffers();


        /**
        *   \brief         Called when an event with an event code unknown to GTEngine is handled.
        *   \param  e [in] A reference to the event that needs to be handled.
        */
        virtual void OnHandleEvent(GameEvent &e);

        /**
        *   \brief  Called when the game is paused.
        */
        virtual void OnPause();

        /**
        *   \brief  Called when the game is resumed.
        */
        virtual void OnResume();


    private:

        /**
        *   \brief  Initialises the game.
        *
        *   \remarks
        *       This method will call the OnInitialise() method when game-specific stuff needs to be initialised. It is up to the child class
        *       to ensure OnInitialise() is implemented correctly. By default, OnInitialise() is just an empty method that returns true. If
        *       OnInitialise() returns false, this method will also return false. 'argc' and 'argv' will be parsed to OnInitialise() unmodified.
        */
        bool Initialise(int argc, char **argv);

        /**
        *   \brief  Helper method for initialising the font cache.
        */
        bool InitialiseFonts();

        /**
        *   \brief  Initialises the GUI server.
        *
        *   \remarks
        *       This should be called after InitialiseFonts() because the GUI depends on the font server.
        */
        bool InitialiseGUI();

        /**
        *   \brief  Uninitialise.
        *
        *   \remarks
        *       This will call the OnUninitialise() event.
        */
        void Uninitialise();

        /**
        *   \brief  Performs the main game loop.
        *
        *   \remarks
        *       This function asserts that the game has been initialised.
        */
        void Loop();

        /**
        *   \brief  Starts the next frame.
        *
        *   \remarks
        *       This will start any applicable threads.
        */
        void StartFrame();

        /**
        *   \brief  Ends the current frame.
        *
        *   \remarks
        *       This will block until all threads have finished.
        */
        void EndFrame();

        /**
        *   \brief  Updates the game. This is run on the update thread.
        */
        void Update();

        /**
        *   \brief  Draws the next frame based on the previous update.
        */
        void Draw();

        /**
        *   \brief  Swaps the rendering command buffers of various objects, including the renderer and GUI.
        *
        *   \remarks
        *       This will call the OnSwapRCBuffers() event AFTER the other buffers have been swapped (Renderer, etc).
        */
        void SwapRCBuffers();


    // The methods below are used to handle events.
    private:

        /**
        *   \brief  Handles every event in the event queue.
        *
        *   \remarks
        *       This is run in the update thread, and is the first thing to be executed.
        */
        void HandleEvents();

        void HandleEvent_OnSize(GameEvent &e);
        void HandleEvent_OnMouseMove(GameEvent &e);
        void HandleEvent_OnMouseWheel(GameEvent &e);
        void HandleEvent_OnMouseButtonDown(GameEvent &e);
        void HandleEvent_OnMouseButtonUp(GameEvent &e);
        void HandleEvent_OnMouseButtonDoubleClick(GameEvent &e);
        void HandleEvent_OnKeyPressed(GameEvent &e);
        void HandleEvent_OnKeyReleased(GameEvent &e);
        void HandleEvent_OnKeyDown(GameEvent &e);
        void HandleEvent_OnKeyUp(GameEvent &e);
        void HandleEvent_OnReceiveFocus(GameEvent &e);
        void HandleEvent_OnLoseFocus(GameEvent &e);


    protected:

        /// The GUI FFI.
        struct GUIFFI
        {
            /// A helper for retrieving the SG1Game instance from the script.
            static Game & GetGameObject(GTCore::Script &script);

            static int Close(GTCore::Script &script);
            static int Pause(GTCore::Script &script);
            static int Resume(GTCore::Script &script);
        };


    private:

        /// Keeps track of whether or not the game is initialised.
        bool isInitialised;

        /// Keeps track of whether or not the game is closing.
        bool closing;

        /// The list of events that are queued and ready for processing.
        GameEventQueue eventQueue;

        /// The mutex for protecting access to the event queue.
        GTCore::Mutex eventQueueLock;

        /// The game's window.
        GTCore::Window *window;

        /// The event handler for the main window.
        GameWindowEventHandler windowEventHandler;

        /// The array of threads that the game will use. There will always be at least one thread. That thread will be the
        /// update thread. The renderer will run on the main thread.
        GTCore::Thread *threads;

        /// A pointer to the update thread. This will point to a member in the 'threads' array.
        GTCore::Thread *updateThread;

        /// The job that will do the game updates.
        GTCore::Threading::Job *updateJob;

        /// The time between the last two frames. Use this for time-based operations.
        double deltaTimeInSeconds;

        /// The timer for timing updates. This is needed for retrieving the delta time.
        GTCore::Timer updateTimer;

        /// The font cache to use with this game.
        GTType::FontServer *fontServer;

        /// The generic font we'll use for basically everything.
        GTType::Font *defaultFont;

        /// The GUI of this game. There is only a single GUI for every game. A game should dynamically show and hide root elements
        /// to show different GUI's for different game states. We use a pointer here because we need to pass it a font server to
        /// it's constructor, which will not be constructed until Game::Initialise().
        GTGUI::Server *gui;

        /// The event handler for the GUI. This is constructed at the same time as 'gui'.
        GTGUI::ServerEventHandler *guiEventHandler;

        /// Whether or not the game is paused. A paused game will continue to render, but scene nodes and animations will not tick.
        bool paused;

        /// Whether or not the game window has focus.
        bool focused;

        /// A map of key states. This is modified as keys are pushed and released.
        GTCore::Map<char32_t, bool> keyDownMap;


        bool mouseCaptured;
        int mouseCapturePosX;
        int mouseCapturePosY;

        int mouseCenterX;
        int mouseCenterY;
        int mousePosXBuffer[MouseBufferSize];
        int mousePosYBuffer[MouseBufferSize];
        size_t mousePosBufferIndex;


    private:    // No copying.
        Game(const Game &);
        Game & operator=(const Game &);


    // Friends.
    friend class GameUpdateJob;     // <-- GameUpdateJob is basically a part of this class...
    };
}

#endif