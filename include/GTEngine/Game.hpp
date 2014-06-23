// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Game_hpp_
#define __GTEngine_Game_hpp_

#include "GameEventQueue.hpp"
#include "GameEventFilter.hpp"
#include "GameWindowEventHandler.hpp"
#include "GameScript.hpp"
#include "Editor.hpp"
#include "GUIEventHandler.hpp"
#include "GameUpdateJob.hpp"
#include "DataFilesWatcher.hpp"
#include "GameState.hpp"
#include "Profiler.hpp"
#include "GUIImageManager.hpp"
#include "Rendering/DefaultGUIRenderer.hpp"
#include "GameStateManager.hpp"
#include <GTLib/Threading.hpp>
#include <GTLib/Timing.hpp>
#include <GTLib/CommandLine.hpp>
#include <GTLib/FontServer.hpp>
#include <GTLib/GUI/Server.hpp>

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

        /// Constructor.
        ///
        /// @remarks
        ///     The constructor is where all initialisation takes place. Use operator bool or IsInitialised() to check if initialisation was successful.
        ///     @par
        ///     If initialisation was not successful, Run() will return immediately. Use Run() to start running the game.
        Game(GameStateManager &gameStateManager);

        /// Destructor.
        ///
        /// @remarks
        ///     This is where everything will be uninitialised.
        virtual ~Game();


        /// Starts up the game.
        ///
        /// @remarks
        ///     This will call OnLoadConfigs() and OnStartup().
        ///     @par
        ///     Client application should not call this method directly. It will instead be called internally by GTEngine::Startup().
        bool Startup(const GTLib::CommandLine &commandLine);

        /// Shuts down the game.
        ///
        /// @remark
        ///     Client applications should not call this method directly. It will instead be called internally by GTEngine::Shutdown().
        void Shutdown();


        /// Runs the game.
        ///
        /// @return An exit code. Zero if the game was run and closed as expected. Non-zero if an error occurs.
        ///
        /// @remarks
        ///     This method will do everything including initialisation/uninitialisation.
        int Run();

        /// Determines whether or not the game is initialised.
        ///
        /// @return True if the game was initialised successfully; false otherwise.
        virtual bool IsInitialised() const { return this->isInitialised; }

        /// Marks the game for closure.
        ///
        /// @remarks
        ///     This does not immediately shutdown the game. Instead it marks the game as wanting to close. It will return before
        ///     the game has actually closed.
        void Close();


        /// Retrieves the absolute path of the directory containing the executable.
        ///
        /// @remarks
        ///     The return value will not include the executable file name.
        const char* GetExecutableDirectoryAbsolutePath() const;

        /// Retrieves the absolute path of the executable file.
        const char* GetExecutableAbsolutePath() const;


        /// Sends an event to the game.
        ///
        /// @remarks
        ///     This function is thread-safe.
        void SendEvent(const GameEvent &e);
        
        
        /// Sets the event filter to filter events with.
        ///
        /// @param filter [in] A reference to the new filter. Can be null, in which case all events will be dispatched without filtering.
        void SetEventFilter(GameEventFilter* filter);
        void SetEventFilter(GameEventFilter &filter) { this->SetEventFilter(&filter); }
        
        /// Retrieves a pointer to the current event filter.
        GameEventFilter* GetEventFilter();
        


        /// Retrieves a reference to the internal GUI server.
        GTGUI::Server & GetGUI();

        /// Steps the GUI by the given delta.
        void StepGUI();
        void StepGUI(double deltaTimeInSeconds);


        /// Retrieves the GUI element of the main game window.
        GTGUI::Element* GetGameWindowGUIElement() { return this->gameWindowGUIElement; }

        /// Retrieves a pointer to the main game window.
        GTLib::Window* GetWindow() { return this->window; }


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



        /// Retrieves the current position of the mouse. This is relative to the main game window.
        void GetMousePosition(int &x, int &y);

        /// Retrieves the offset of the mouse when captured.
        ///
        /// @remarks
        ///     When the mouse is captured, you use an offset instead of an absolute position. This is the function to use
        ///     when the mouse offset is needed.
        ///     @par
        ///     This will use a smoothed value is mouse smoothing is enabled.
        void GetMouseOffset(float &x, float &y);

        /// Sets the position of the mouse relative to the game's main window.
        void SetMousePosition(int x, int y);

        /// Enables mouse smoothing.
        void EnableMouseSmoothing();

        /// Disables mouse smoothing.
        void DisableMouseSmoothing();

        /// Determines whether or not mouse smoothing is enabled.
        bool IsMouseSmoothingEnabled() const;


        /// Retrieves the scripting environment of the game.
              GameScript & GetScript()       { return this->script; }
        const GameScript & GetScript() const { return this->script; }
        


        /// Retrieves the time in seconds between the last two frames. Allows time based movement.
        double GetDeltaTimeInSeconds() const { return this->deltaTimeInSeconds; }

        /// Retrieves the time in seconds since the game was launched, not including pause time.
        ///
        /// @remarks
        ///     This is useful for doing timer-based stuff where pausing the game should not be contributing to the timer.
        double GetTimeInSeconds() const { return this->totalRunninTimeInSeconds; }



        /// Captures the mouse.
        void CaptureMouse();

        /// Releases the mouse.
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
        bool IsKeyDown(GTLib::Key key) const;

        /// Determines if a mouse button is currently pressed.
        ///
        /// @param button [in] The mouse button code to check.
        ///
        /// @return True if the mouse button is currently held down.
        bool IsMouseButtonDown(GTLib::MouseButton button) const;


        /// Pauses the game.
        ///
        /// @remarks
        ///     When the game is paused, scene nodes will continue to render, but nothing will be stepped.
        ///     @par
        ///     This will also pause the current scene. An application should never need to explicately pause/resume a scene.
        void Pause();

        /// Resumes the paused game.
        ///
        /// @remarks
        ///     This will also resume the current scene. An application should never need to explicately pause/resume a scene.
        void Resume();

        /// Determines whether or not the game is paused.
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


        /// Generically activates the given game state.
        ///
        /// @param newGameState [in] A reference to the game state to activate.
        void ActivateGameState(GameState &newGameState);

        /// Activates the previous game state.
        void ActivatePreviousGameState();

        /// Deactivates the current game state.
        ///
        /// @remarks
        ///     This will leave the game without a current game state, but will set the previous state. This is good for enabling the editor.
        void DeactivateCurrentGameState();

        /// Retrieves the currentnly active game state.
        GameState* GetCurrentGameState() { return this->currentGameState; }



        /// Enables automatic script reloading.
        ///
        /// This will be disabled by default. Scripts will be reloaded when the game window receives focus.
        void EnableScriptAutoReload();

        /// Disables automatic script reloading.
        void DisableScriptAutoReload();

        /// Determines whether or not automatic script reloading is enabled.
        bool IsScriptAutoReloadEnabled() const { return this->isAutoScriptReloadEnabled; }



        /// Retrieves the profiler.
              Profiler & GetProfiler()       { return this->profiler; }
        const Profiler & GetProfiler() const { return this->profiler; }



        /// Packages the game for distribution.
        ///
        /// @param outputDirectory [in] The output directory, relative to the running directory.
        /// @param executableName  [in] The name of the executable.
        bool PackageForDistribution(const char* outputDirectory, const char* executableName);



        ///////////////////////////////////////////////
        // Events from Files Watcher.

        /// Called when a file is added.
        void OnFileInsert(const DataFilesWatcher::Item &item);

        /// Called when a file is removed.
        void OnFileRemove(const DataFilesWatcher::Item &item);

        /// Called when a file is updated.
        void OnFileUpdate(const DataFilesWatcher::Item &item);


       
        ///////////////////////////////////////////////
        // Key Bindings.

        /// Retrieves the key for toggling the profiler.
        GTLib::Key GetProfilerToggleKey() const;

        /// Sets the key for toggling the profiler.
        void SetProfilerToggleKey(GTLib::Key key);


        /// Retrieves the key combination for toggling the editor.
        const GTLib::KeyCombination & GetEditorToggleKeyCombination() const;

        /// Sets the key combination for toggling the editor.
        void SetEditorToggleKeyCombination(const GTLib::KeyCombination &newCombination);




        /// A virtual function that creates an instantiation of a custom component.
        ///
        /// @param componentName [in] The name of the component to instantiate.
        /// @param hostSceneNode [in] The scene node that will become the owner of the new component.
        virtual Component* CreateCustomComponent(const char* componentName, SceneNode &hostSceneNode) const;




        /////////////////////////////////////
        // Game State Management

        /// Called when the game state needs to be serialized.
        ///
        /// @param serializer [in] A reference to the serializer to write the data to.
        ///
        /// @return True if the game state is serialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This should not be called directly. This is simply a pass-through to the Serialize() method of the game's state manager.
        bool SerializeGameState(GTLib::Serializer &serializer);

        /// Called when the game state needs to be deserialized.
        ///
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        ///
        /// @return True if the game state is deserialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This should not be called directly. This is simply a pass-through to the Deserialize() method of the game's state manager.
        bool DeserializeGameState(GTLib::Deserializer &deserializer);

        /// A helper method for saving a game state to a file (doing a save game).
        ///
        /// @param destinationFilePath [in] The relative or absolute path of the file to save the game state to.
        ///
        /// @return True if the game was saved successfully; false otherwise.
        ///
        /// @remarks
        ///     If you want simple save game functionality, use this method.
        bool SaveGameState(const char* destinationFilePath);

        /// A helper method for loading a game state from a file (doing a load game).
        ///
        /// @param sourceFilePath [in] The relative or absolute path of the file to load the game state from.
        ///
        /// @return True if the game was loaded successfully; false otherwise.
        ///
        /// @remarks
        ///     If you want simple load game functionality, use this method.
        bool LoadGameState(const char* sourceFilePath);

        /// Loads a scene.
        ///
        /// @param sceneRelativePath [in] The relative path of the scene to load.
        ///
        /// @return True if the scene is loaded and switched successfully; false otherwise.
        ///
        /// @remarks
        ///     This is actually just a pass-through to the LoadScene() method of the game's state manager.
        bool LoadScene(const char* sceneRelativePath);


        ////////////////////////////////////
        // Public Game State Management Events.

        /// Called when a scene is loaded via the scripting environment.
        ///
        /// @param sceneRelativePath [in] The relative path of the scene that wants to load.
        ///
        /// @return True if the scene is loaded and switched successfully; false otherwise.
        ///
        /// @remarks
        ///     Games should handle this themselves to enable support for loading scenes via the scripting environment.
        ///     @par
        ///     Do not call this directly.
        //virtual bool OnLoadScene(const char* sceneRelativePath);

        


    protected:

        void CacheMousePosition();


#if 0
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
        virtual bool OnStartup(const GTLib::CommandLine &commandLine);

        /// Called when the game is shutting down.
        virtual void OnShutdown();


        /// Called when the game is updated. This is called from the update thread.
        ///
        /// @param deltaTimeInSeconds [in] The time since the last frame, in seconds.
        virtual void OnUpdate(double deltaTimeInSeconds);

        /**
        *   \brief  Called on the rendering thread just before the main scene is drawn.
        */
        virtual void OnDraw();

        /**
        *   \brief  Called on the rendering thread after the main rendering commands have been drawn, and just before swapping the buffers.
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
        virtual void OnMouseButtonDown(GTLib::MouseButton button, int x, int y);
        virtual void OnMouseButtonUp(GTLib::MouseButton button, int x, int y);
        virtual void OnMouseButtonDoubleClick(GTLib::MouseButton button, int x, int y);
        virtual void OnKeyPressed(GTLib::Key key);
        virtual void OnKeyReleased(GTLib::Key key);
        virtual void OnKeyDown(GTLib::Key key);
        virtual void OnKeyUp(GTLib::Key key);
        virtual void OnReceiveFocus();
        virtual void OnLoseFocus();


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
#endif

    private:


        /**
        *   \brief  Initialises the GUI server.
        *
        *   \remarks
        *       This should be called after InitialiseFonts() because the GUI depends on the font server.
        */
        bool InitialiseGUI();

        /// Helper method for starting up the data files watcher.
        ///
        /// @return True if the data files watcher was initialised successfully.
        bool InitialiseDataFilesWatcher();

        
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



        /// Determines whether or not all of the keys in the given key combination are down.
        bool IsKeyCombinationDown(const GTLib::KeyCombination &combination) const;


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
        void HandleEvent_OnMouseMove(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnMouseWheel(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnMouseButtonDown(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnMouseButtonUp(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnMouseButtonDoubleClick(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnKeyPressed(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnKeyReleased(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnKeyDown(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnKeyUp(GameEvent &e, GTGUI::EventContext eventContext);
        void HandleEvent_OnReceiveFocus(GameEvent &e);
        void HandleEvent_OnLoseFocus(GameEvent &e);


        /// Helper for posting the OnUpdate event to the scripting environment.
        void PostScriptEvent_OnUpdate(double deltaTimeInSeconds);
        void PostScriptEvent_OnSize(const GameEvent &e);
        void PostScriptEvent_OnMouseMove(const GameEvent &e);
        void PostScriptEvent_OnMouseWheel(const GameEvent &e);
        void PostScriptEvent_OnMouseButtonDown(const GameEvent &e);
        void PostScriptEvent_OnMouseButtonUp(const GameEvent &e);
        void PostScriptEvent_OnMouseButtonDoubleClick(const GameEvent &e);
        void PostScriptEvent_OnKeyPressed(const GameEvent &e);
        void PostScriptEvent_OnKeyReleased(const GameEvent &e);
        void PostScriptEvent_OnKeyDown(const GameEvent &e);
        void PostScriptEvent_OnKeyUp(const GameEvent &e);
        void PostScriptEvent_OnReceiveFocus(const GameEvent &e);
        void PostScriptEvent_OnLoseFocus(const GameEvent &e);

        /// Helper for posting the OnSerializeGameState event to the scripting environment.
        ///
        /// @param serializer [in] A reference to the serializer to pass to the event handlers.
        void PostScriptEvent_OnSerializeGameState(GTLib::Serializer &serializer);

        /// Helper for posting the OnDeserializeGameState event to the scripting environment.
        ///
        /// @param deserializer [in] A reference to the deserializer to pass to the event handlers.
        void PostScriptEvent_OnDeserializeGameState(GTLib::Deserializer &deserializer);



    private:

        /// The game state manager.
        GameStateManager &m_gameStateManager;


        /// Keeps track of whether or not the game is initialised.
        bool isInitialised;

        /// Keeps track of whether or not the game is closing.
        bool closing;

        
        /// The absolute path of the executable.
        GTLib::String executablePath;

        /// The absolute path of the directory containing the executable.
        GTLib::String executableDirectoryPath;


        /// The list of events that are queued and ready for processing.
        GameEventQueue eventQueue;

        /// The mutex for protecting access to the event queue.
        GTLib::Mutex eventQueueLock;
        
        /// A pointer to the event filter to filter events with. This can be null, in which case events will be dispatched without filtering. Defaults to null.
        GameEventFilter* eventFilter;

        /// The game's window.
        GTLib::Window* window;

        /// The event handler for the main window.
        GameWindowEventHandler windowEventHandler;


        /// The scripting environment for doing anything with scripts.
        mutable GameScript script;


        /// A pointer to the update thread. This will point to a member in the 'threads' array.
        GTLib::Thread* updateThread;

        /// The job that will do the game updates.
        GameUpdateJob updateJob;


        /// The time between the last two frames. Use this for time-based operations.
        double deltaTimeInSeconds;

        /// The amount of time the game has been running, not including pause time.
        double totalRunninTimeInSeconds;


        /// The timer for timing updates. This is needed for retrieving the delta time.
        GTLib::Timer updateTimer;
        

        /// The GUI image manager.
        GUIImageManager guiImageManager;

        /// The GUI of this game. There is only a single GUI for every game. A game should dynamically show and hide root elements
        /// to show different GUI's for different game states.
        GTGUI::Server gui;

        /// The event handler for the GUI.
        GUIEventHandler guiEventHandler;

        /// The GUI renderer.
        DefaultGUIRenderer guiRenderer;

        /// The GUI element for the main game window.
        GTGUI::Element* gameWindowGUIElement;


        /// Whether or not the game is paused. A paused game will continue to render, but scene nodes and animations will not tick.
        bool paused;

        /// Whether or not the game window has focus.
        bool focused;

        /// Whether or not the cursor is visible.
        bool isCursorVisible;

        /// Keeps track of whether or not automatic script reloading is enabled. False by default.
        bool isAutoScriptReloadEnabled;


        /// A map of key states. This is modified as keys are pushed and released.
        GTLib::Map<char32_t, bool> keyDownMap;

        /// A map containing mouse button states. This is modified as mouse buttons are pushed and released. These are index by
        /// the GTLib::MouseButton enum.
        GTLib::Map<GTLib::MouseButton, bool> mouseButtonDownMap;


        /// Class representing the editor.
        Editor editor;


        /// The profiler.
        Profiler profiler;


        /// Structure containing the debuggin GUI elements.
        struct _DebuggingGUI
        {
            _DebuggingGUI(Game &gameIn)
                : game(gameIn), gui(nullptr),
                  DebuggingMain(nullptr),
                  FPSValue(nullptr), UpdateTime(nullptr), RenderTime(nullptr),
                  updateIntervalInSeconds(0.5), lastUpdateTime(0.0),
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
                        this->UpdateTime    = this->gui->GetElementByID("Profiler_UpdateTime");
                        this->RenderTime    = this->gui->GetElementByID("Profiler_RenderTime");

                        this->isInitialised = true;
                    }
                }

                return this->isInitialised;
            }

            // Updates the debugging information. This should be called once per frame.
            void Update(Profiler &profiler)
            {
                if (GTLib::Timing::GetTimeInSeconds() - this->lastUpdateTime >= this->updateIntervalInSeconds)
                {
                    this->lastUpdateTime = GTLib::Timing::GetTimeInSeconds();

                    double delta = profiler.GetAverageFrameTime();
                    double fps   = 0.0;

                    if (delta > 0.0)
                    {
                        fps = 1.0 / profiler.GetAverageFrameTime();
                    }

                    char valueStr[64];
                    GTLib::IO::snprintf(valueStr, 64, "%.1f / %.4f", fps, delta * 1000.0);

                    if (this->FPSValue != nullptr)
                    {
                        this->FPSValue->SetText(valueStr);
                    }



                    // Update and Render time.
                    double updateTime = profiler.GetAverageUpdateTime();
                    double renderTime = profiler.GetAverageRenderingTime();

                    GTLib::IO::snprintf(valueStr, 64, "%.4f (%.1f%%)", updateTime * 1000, updateTime / delta * 100.0);
                    this->UpdateTime->SetText(valueStr);

                    GTLib::IO::snprintf(valueStr, 64, "%.4f (%.1f%%)", renderTime * 1000, renderTime / delta * 100.0);
                    this->RenderTime->SetText(valueStr);
                }
            }


            /// Shows the debugging GUI.
            void Show()
            {
                if (this->DebuggingMain != nullptr)
                {
                    this->DebuggingMain->Show();
                }
            }

            /// Hides the debugging GUI.
            void Hide()
            {
                if (this->DebuggingMain != nullptr)
                {
                    this->DebuggingMain->Hide();
                }
            }


            /// The game that owns this structure.
            Game &game;


            /// A pointer to the main GUI server this debugging GUI is part of. This is set in Initialise().
            GTGUI::Server* gui;

            /// The main editor element. Every single element used by the editor is a child of this element.
            GTGUI::Element* DebuggingMain;

            /// The FPSValue element.
            GTGUI::Element* FPSValue;

            /// The update time element.
            GTGUI::Element* UpdateTime;

            /// The render time element.
            GTGUI::Element* RenderTime;


            /// The amount of time to wait to update the debug information, in seconds.
            double updateIntervalInSeconds;

            /// The last time the GUI was updated.
            double lastUpdateTime;


            /// Whether or not the debugging GUI has been initialised.
            bool isInitialised;

            /// Whether or not the debugging GUI is currently showing.
            bool isShowing;


        private:
            _DebuggingGUI(const _DebuggingGUI &);
            _DebuggingGUI & operator=(const _DebuggingGUI &);

        }DebuggingGUI;


        bool isMouseSmoothingEnabled;
        bool mouseCaptured;
        bool captureMouseOnReceiveFocus;            ///< Keeps track of whether or not to capture the mouse when the main window receives focus.
        bool captureMouseOnEditorClose;             ///< Keeps track of whether or not to capture the mouse when the editor is closed. 
        int mouseCapturePosX;
        int mouseCapturePosY;

        int mouseCenterX;
        int mouseCenterY;
        int mousePosXBuffer[MouseBufferSize];
        int mousePosYBuffer[MouseBufferSize];
        size_t mousePosBufferIndex;


        /// The x position of the mouse.
        int mousePosX;

        /// The y position of the mouse.
        int mousePosY;

        /// Incrementing this will cause that number of mouse move events to be discarded. Really useful for locking movement events from mouse captures.
        int mouseMoveLockCounter;


        /// Object use for watching changes to the data directories. Mainly used for the editor.
        DataFilesWatcher dataFilesWatcher;

        /// The last time the data files where checked.
        float lastDataFilesWatchTime;

        /// Controls whether or not the data directories should be dynamically watched. This should only really need to be enabled when running tools like the editor.
        bool isDataFilesWatchingEnabled;


        /// The current game state.
        GameState* currentGameState;

        /// The previous game state (for toggling).
        GameState* previousGameState;


        /// The event handler for the data files watcher. This will just dispatch the events to the main Game object.
        class DataFilesWatcherEventHandler : public DataFilesWatcher::EventHandler
        {
        public:

            /// Constructor.
            DataFilesWatcherEventHandler(Game &game)
                : game(game)
            {
            }


            void OnInsert(const DataFilesWatcher::Item &item) { this->game.OnFileInsert(item); }
            void OnRemove(const DataFilesWatcher::Item &item) { this->game.OnFileRemove(item); }
            void OnUpdate(const DataFilesWatcher::Item &item) { this->game.OnFileUpdate(item); }

        private:

            /// The editor object that owns this event handler.
            Game &game;

        private:    // No copying.
            DataFilesWatcherEventHandler(const DataFilesWatcherEventHandler &);
            DataFilesWatcherEventHandler & operator=(const DataFilesWatcherEventHandler &);

        }dataFilesWatcherEventHandler;




        /// The key binding for toggling the profiler.
        GTLib::Key profilerToggleKey;


        /// The key combination for toggling the editor.
        GTLib::KeyCombination editorToggleKeyCombination;



    private:    // No copying.
        Game(const Game &);
        Game & operator=(const Game &);


    friend class GameUpdateJob;     // <-- GameUpdateJob is basically a part of this class...
    };
}

#endif
