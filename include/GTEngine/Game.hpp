
#ifndef __GTEngine_Game_hpp_
#define __GTEngine_Game_hpp_

#include "GameEventQueue.hpp"
#include "GameWindowEventHandler.hpp"
#include "GameScript.hpp"
#include "Editor.hpp"
#include "GUIEventHandler.hpp"
#include "GameUpdateJob.hpp"
#include "DataFilesWatcher.hpp"
#include "Rendering/RenderCommands/RCSetFramebuffer.hpp"
#include <GTCore/Threading.hpp>
#include <GTCore/Timing.hpp>
#include <GTType/FontServer.hpp>
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    static const size_t MouseBufferSize   = 2;
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
        Game();

        /**
        *   \brief  Destructor.
        *
        *   \remarks
        *       This is where everything will be uninitialised.
        */
        virtual ~Game();


        /// Starts up the game.
        ///
        /// @remarks
        ///     This will call OnLoadConfigs() and OnStartup().
        ///     @par
        ///     Client application should not call this method directly. It will instead be called internally by GTEngine::Startup().
        bool Startup(int argc, char** argv);

        /// Shuts down the game.
        ///
        /// @remark
        ///     Client applications should not call this method directly. It will instead be called internally by GTEngine::Shutdown().
        void Shutdown();


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
        GTCore::Window* GetWindow() { return this->window; }


        /// Enables fullscreen mode.
        void EnableFullscreen();

        /// Disables fullscreen mode.
        void DisableFullscreen();

        /// Determines whether or not the window is fullscreen.
        bool IsFullscreen() const;


        /// Refreshes the display settings such as the window size and fullscreen mode.
        ///
        /// @remarks
        ///     This refreshes based on the current script settings.
        void RefreshDisplay();



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


        /// Retrieves the scripting environment of the game.
              GameScript & GetScript()       { return this->script; }
        const GameScript & GetScript() const { return this->script; }
        


        /**
        *   \brief  Retrieves the time in seconds between the last two frames. Allows time based movement.
        */
        double GetDeltaTimeInSeconds() const { return this->deltaTimeInSeconds; }

        /// Retrieves the time in seconds since the game was launched, not including pause time.
        ///
        /// @remarks
        ///     This is useful for doing timer-based stuff where pausing the game should not be contributing to the timer.
        double GetTimeInSeconds() const { return this->totalRunninTimeInSeconds; }


        /**
        *   \brief  Retrieves a pointer to the games font cache.
        */
        GTType::FontServer & GetFontServer() { return this->fontServer; }

        /**
        *   \brief  Retrieves a pointer to the default font.
        */
        GTType::Font* GetDefaultFont() { return this->defaultFont; }

        /**
        *   \brief  Captures the mouse.
        */
        void CaptureMouse();

        /**
        *   \brief  Releases the mouse.
        */
        void ReleaseMouse();

        /// Determines whether or not the mouse is captured.
        bool IsMouseCaptured() const { return this->mouseCaptured; }


        /// Shows the mouse cursor.
        void ShowCursor();

        /// Hides the mouse cursor.
        void HideCursor();


        /// Determines if a key is currently pressed.
        ///
        /// @param key [in] The key code to check.
        ///
        /// @return True if the key is currently held down.
        bool IsKeyDown(GTCore::Key key) const;

        /// Determines if a mouse button is currently pressed.
        ///
        /// @param button [in] The mouse button code to check.
        ///
        /// @return True if the mouse button is currently held down.
        bool IsMouseButtonDown(GTCore::MouseButton button) const;


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


        /// Retrieves a reference to the editor object.
              Editor & GetEditor()       { return this->editor; }
        const Editor & GetEditor() const { return this->editor; }

        /// Opens the editor.
        void OpenEditor();

        /// Closes the editor.
        void CloseEditor();

        /// Determines whether or not the editor is open.
        bool IsEditorOpen() const;


        /// Shows the debugging overlay.
        void ShowDebugging();

        /// Hides the debugging overlay.
        void HideDebugging();

        /// Determines whether or now the debugging overlay is shown.
        bool IsDebuggingOpen() const;


        /// Executes the given script on the central game script.
        ///
        /// @param script [in] The script string to execute. <b>This is not a file name</b>
        ///
        /// @return True if the script is executed successfully; false otherwise.
        bool ExecuteScript(const char* script);



        /// Retrieves a reference to the data files watcher.
        DataFilesWatcher & GetDataFilesWatcher() { return this->dataFilesWatcher; }

        /// Enables watching of the data files.
        void EnableDataFilesWatching() { this->isDataFilesWatchingEnabled = true; }

        /// Disables watching of the data files.
        void DisableDataFilesWatching() { this->isDataFilesWatchingEnabled = false; }

        /// Determines whether or not the data files are being watched.
        bool IsDataFilesWatchingEnabled() const { return this->isDataFilesWatchingEnabled; }

        /// Retrieves the interval for checking for changes to the data files.
        float GetDataFilesWatchInterval() const
        {
            float interval = this->script.GetFloat("Game.DataFilesWatchInterval");
            if (interval == 0.0f)
            {
                interval = 2.0f;
            }

            return interval;
        }



    protected:

        void CacheMousePosition();


        
    protected:  // Event handlers.


        /// Called when the config scripts are being loaded.
        ///
        /// @remarks
        ///     This is called right at the start of initialisation just before the --config arguments are processed. This is the
        ///     first event to be called, before OnStartup() and before anything else is initialised.
        ///     @par
        ///     This is the best place to load configs. However, since this function is called before everything is initialised,
        ///     the configs should only use simple script-side stuff. Complex scripts should be run in OnStartup().
        virtual void OnLoadConfigs();


        /// Called when the game is just finishing up starting up.
        ///
        /// @remarks
        ///     This is called after the window has been created and shown.
        ///     @par
        ///     The return value is true if the startup is successful; false otherwise.
        virtual bool OnStartup(int argc, char** argv);

        /// Called when the game is shutting down.
        virtual void OnShutdown();


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


        // TODO: Check if this is still needed.
        /**
        *   \brief  Called when any rendering RC queues need to be swapped.
        */
        virtual void OnSwapRCQueues();


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


        /// Called just before the editor is trying to be opened.
        ///
        /// @return True to let the editor continue opening; false otherwise. Returns true by default.
        virtual bool OnEditorOpening();

        /// Called just before the editor is closing.
        ///
        /// @return True to let the editor continue closing; false otherwise. Returns true by default.
        virtual bool OnEditorClosing();

        /// Called just after the editor has been opened.
        virtual void OnEditorOpen();

        /// Called just after the editor has been closed.
        virtual void OnEditorClose();

        


    private:


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
        *   \brief  Swaps the RC queues of various objects, including the renderer and GUI.
        *
        *   \remarks
        *       This will call the OnSwapRCQueues() event AFTER the other queues have been swapped (Renderer, etc).
        */
        void SwapRCQueues();


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
        GTCore::Window* window;

        /// The event handler for the main window.
        GameWindowEventHandler windowEventHandler;


        /// The scripting environment for doing anything with scripts.
        mutable GameScript script;


        /// A pointer to the update thread. This will point to a member in the 'threads' array.
        GTCore::Thread* updateThread;

        /// The job that will do the game updates.
        GameUpdateJob updateJob;


        /// The time between the last two frames. Use this for time-based operations.
        double deltaTimeInSeconds;

        /// The amount of time the game has been running, not including pause time.
        double totalRunninTimeInSeconds;


        /// The timer for timing updates. This is needed for retrieving the delta time.
        GTCore::Timer updateTimer;

        /// The font server to use with this game.
        GTType::FontServer fontServer;

        /// The generic font we'll use for basically everything.
        GTType::Font* defaultFont;

        /// The GUI of this game. There is only a single GUI for every game. A game should dynamically show and hide root elements
        /// to show different GUI's for different game states.
        GTGUI::Server gui;

        /// The event handler for the GUI.
        GUIEventHandler guiEventHandler;

        /// Whether or not the game is paused. A paused game will continue to render, but scene nodes and animations will not tick.
        bool paused;

        /// Whether or not the game window has focus.
        bool focused;

        /// Whether or not the cursor visibility state has changed.
        bool cursorVisibleChanged;

        /// Whether or not the cursor is visible.
        bool isCursorVisible;


        /// A map of key states. This is modified as keys are pushed and released.
        GTCore::Map<char32_t, bool> keyDownMap;

        /// A map containing mouse button states. This is modified as mouse buttons are pushed and released. These are index by
        /// the GTCore::MouseButton enum.
        GTCore::Map<GTCore::MouseButton, bool> mouseButtonDownMap;


        /// Class representing the editor.
        Editor editor;


        /// Structure containing the debuggin GUI elements.
        struct _DebuggingGUI
        {
            _DebuggingGUI()
                : gui(nullptr),
                  DebuggingMain(nullptr),
                  FPSValue(nullptr), DeltaValue(nullptr),
                  updateIntervalInSeconds(0.5), timer(), totalStepCount(0), averageDelta(0.0),
                  isInitialised(false), isShowing(false)
            {
            }

            bool Initialise(GTGUI::Server &gui)
            {
                if (!this->isInitialised)
                {
                    this->gui = &gui;

                    if (this->gui->LoadFromFile("engine/gui/debugging.xml"))
                    {
                        this->DebuggingMain = this->gui->GetElementByID("DebuggingMain");
                        this->FPSValue      = this->gui->GetElementByID("FPSValue");
                        this->DeltaValue    = this->gui->GetElementByID("DeltaValue");

                        this->isInitialised = true;
                    }
                }

                return this->isInitialised;
            }

            // Updates the debugging information. This should be called once per frame.
            // Returns true if the debugging information was refreshed based on the update interval. False otherwise.
            void Step()
            {
                if (this->timer.GetTimeSinceLastUpdate() >= updateIntervalInSeconds)
                {
                    this->averageDelta   = this->timer.GetTimeSinceLastUpdate() / static_cast<double>(GTCore::Max(this->totalStepCount, 1U));
                    this->totalStepCount = 0;

                    this->timer.Update();

                    char tempStr[64];
                    if (this->FPSValue != nullptr)
                    {
                        GTCore::IO::snprintf(tempStr, 64, "%.1f", 1.0 / this->averageDelta);
                        this->FPSValue->SetText(tempStr);
                    }

                    if (this->DeltaValue != nullptr)
                    {
                        GTCore::IO::snprintf(tempStr, 64, "%f", this->averageDelta);
                        this->DeltaValue->SetText(tempStr);
                    }
                }

                ++this->totalStepCount;
            }

            /// A pointer to the main GUI server this debugging GUI is part of. This is set in Initialise().
            GTGUI::Server* gui;

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* DebuggingMain;

            /// The FPSValue element.
            GTGUI::Element* FPSValue;

            /// The DeltaValue element.
            GTGUI::Element* DeltaValue;


            /// The amount of time to wait to update the debug information, in seconds.
            double updateIntervalInSeconds;

            /// The timer for keeping track of the debugging interval.
            GTCore::Timer timer;

            /// The number of steps that were performed during the sum of 'totalFPS'.
            unsigned int totalStepCount;

            /// The averages FPS based on totalFPSTime and totalFPSCount.
            double averageDelta;


            /// Whether or not the debugging GUI has been initialised.
            bool isInitialised;

            /// Whether or not the debugging GUI is currently showing.
            bool isShowing;

        }DebuggingGUI;


        bool mouseCaptured;
        int mouseCapturePosX;
        int mouseCapturePosY;

        int mouseCenterX;
        int mouseCenterY;
        int mousePosXBuffer[MouseBufferSize];
        int mousePosYBuffer[MouseBufferSize];
        size_t mousePosBufferIndex;


        /// Object use for watching changes to the data directories. Mainly used for the editor.
        DataFilesWatcher dataFilesWatcher;

        /// The last time the data files where checked.
        float lastDataFilesWatchTime;

        /// Controls whether or not the data directories should be dynamically watched. This should only really need to be enabled when running tools like the editor.
        bool isDataFilesWatchingEnabled;


    private:    // No copying.
        Game(const Game &);
        Game & operator=(const Game &);


    friend class GameUpdateJob;     // <-- GameUpdateJob is basically a part of this class...
    };
}

#endif