// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Context
#define GT_Context

#include "ParticleSystemLibrary.hpp"
#include "PrefabLibrary.hpp"
#include "ModelLibrary.hpp"
#include "MaterialLibrary.hpp"
#include "VertexArrayLibrary.hpp"
#include "ShaderLibrary.hpp"
#include "GameEventQueue.hpp"
#include "GameEventFilter.hpp"
#include "GameWindowEventHandler.hpp"
#include "GameScript.hpp"
#include "Editor.hpp"
#include "GUIEventHandler.hpp"
#include "Profiler.hpp"
#include "GameStateManager.hpp"

#ifdef _WIN32
#undef GetCommandLine
#endif

namespace GT
{
    static const size_t MouseBufferSize   = 2;
    static const float  MouseSmoothFactor = 0.5f;

    /// Class representing the engine context.
    class Context
    {
    public:

        /// Constructor.
        Context(GameStateManager &gameStateManager);

        /// Destructor.
        ~Context();


        /// Starts up the context from a command line object.
        bool Startup(dr_cmdline &cmdline);

        /// Starts up the context from an argv style command line.
        bool Startup(int argc, char** argv);

        /// Starts up the context from a Win32 style command line.
        bool Startup(const char* cmdline);

        /// Starts up the context without a command line.
        bool Startup();


        /// Shuts down the game.
        void Shutdown();



        ////////////////////////////////////////////////////
        // Command Line

        /// Retrieve a reference to the internal command line object.
        ///
        /// @return A pointer to the internal command line object.
        dr_cmdline & GetCommandLine() { return m_cmdline; }



        ////////////////////////////////////////////////////
        // File System Management

        /// Retrieves a pointer to the virtual file system object.
        drfs_context* GetVFS() { return m_pVFS; }

        /// Retrieves the executable's absolute file path.
        const char* GetExecutableAbsolutePath() const;

        /// Retrieves the absolute path of the executable's directory.
        ///
        /// @remarks
        ///     This does not include the executable's directory
        const char* GetExecutableDirectoryAbsolutePath() const;

        /// Adds a base directory relative to the executable.
        void AddBaseDirectoryRelativeToExe(const char* relativePath);



        ////////////////////////////////////////////////////
        // Messages

        /// Posts a simple log message.
        void Log(const char* message);

        /// Posts a generic, formatted message.
        void Logf(const char* format, ...);

        /// Posts a message.
        void LogMessage(const char* message);
        void LogMessagef(const char* format, ...);

        /// Posts a warning message.
        void LogWarning(const char* message);
        void LogWarningf(const char* format, ...);

        /// Posts an error message.
        void LogError(const char* message);
        void LogErrorf(const char* format, ...);




        ////////////////////////////////////////////////////
        // Audio

        /// Retrieves a pointer to the dr_audio context.
        dra_context* GetAudioContext();

        /// Retrieves a pointer to the audio playback device.
        dra_device* GetAudioPlaybackDevice();

        /// Retrieves a reference to the global sound world.
        ///
        /// @return A reference to the global sound world for when a sound needs to be played globally, outside of a scene.
        GT::SoundWorld & GetSoundWorld();



        ////////////////////////////////////////////////////
        // Assets

        /// Returns a reference to the internal asset library.
        AssetLibrary & GetAssetLibrary();

        /// Retrieves a reference to the internal script library.
        ScriptLibrary & GetScriptLibrary() { return m_scriptLibrary; }

        /// Retrieves a reference to the internal particle system library.
        ParticleSystemLibrary & GetParticleSystemLibrary() { return m_particleSystemLibrary; }

        /// Retrieves a reference to the internal prefab library.
        PrefabLibrary & GetPrefabLibrary() { return m_prefabLibrary; }

        /// Retrieves a reference to the internal model library.
        ModelLibrary & GetModelLibrary() { return m_modelLibrary; }

        /// Retrieves a reference to the internal material library.
        MaterialLibrary & GetMaterialLibrary() { return m_materialLibrary; }

        /// Retrieves a reference to the internal shader library.
        ShaderLibrary & GetShaderLibrary() { return m_shaderLibrary; }

        /// Retrieves a reference to the internal vertex array library.
        VertexArrayLibrary & GetVertexArrayLibrary() { return m_vertexArrayLibrary; }

        /// Retrieves a reference to the internal texture library.
        Texture2DLibrary & GetTextureLibrary() { return m_textureLibrary; }



        //// FROM GAME ////

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
        //const char* GetExecutableDirectoryAbsolutePath() const;

        /// Retrieves the absolute path of the executable file.
        //const char* GetExecutableAbsolutePath() const;


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
        GUIServer & GetGUI();

        /// Steps the GUI by the given delta.
        void StepGUI();
        void StepGUI(double deltaTimeInSeconds);


        /// Retrieves the GUI element of the main game window.
        GUIElement* GetGameWindowGUIElement() { return this->gameWindowGUIElement; }

        /// Retrieves a pointer to the main game window.
        Window* GetWindow() { return this->window; }


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
        bool IsKeyDown(Key key) const;

        /// Determines if a mouse button is currently pressed.
        ///
        /// @param button [in] The mouse button code to check.
        ///
        /// @return True if the mouse button is currently held down.
        bool IsMouseButtonDown(MouseButton button) const;


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


        /// Executes the given script on the central game script.
        ///
        /// @param script [in] The script string to execute. <b>This is not a file name</b>
        ///
        /// @return True if the script is executed successfully; false otherwise.
        bool ExecuteScript(const char* script);




        /// Enables automatic script reloading.
        ///
        /// This will be disabled by default. Scripts will be reloaded when the game window receives focus.
        void EnableScriptAutoReload();

        /// Disables automatic script reloading.
        void DisableScriptAutoReload();

        /// Determines whether or not automatic script reloading is enabled.
        bool IsScriptAutoReloadEnabled() const { return this->isAutoScriptReloadEnabled; }



        /// Packages the game for distribution.
        ///
        /// @param outputDirectory [in] The output directory, relative to the running directory.
        /// @param executableName  [in] The name of the executable.
        bool PackageForDistribution(const char* outputDirectory, const char* executableName);



       
        ///////////////////////////////////////////////
        // Key Bindings.

        /// Retrieves the key for toggling the profiler.
        Key GetProfilerToggleKey() const;

        /// Sets the key for toggling the profiler.
        void SetProfilerToggleKey(Key key);


        /// Retrieves the key combination for toggling the editor.
        const KeyCombination & GetEditorToggleKeyCombination() const;

        /// Sets the key combination for toggling the editor.
        void SetEditorToggleKeyCombination(const KeyCombination &newCombination);




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
        bool SerializeGameState(Serializer &serializer);

        /// Called when the game state needs to be deserialized.
        ///
        /// @param deserializer [in] A reference to the deserializer to read the data from.
        ///
        /// @return True if the game state is deserialized successfully; false otherwise.
        ///
        /// @remarks
        ///     This should not be called directly. This is simply a pass-through to the Deserialize() method of the game's state manager.
        bool DeserializeGameState(Deserializer &deserializer);

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


    protected:

        void CacheMousePosition();


    private:

        /// Determines whether or not "--editor" was passed on the command line.
        bool IsEditorOnCommandLine();


        /// Initialises the GUI server.
        ///
        /// @remarks
        ///     This should be called after InitialiseFonts() because the GUI depends on the font server.
        bool InitialiseGUI();

        /// Updates a renders a single frame.
        void DoFrame();

        /// Updates the game. This is run on the update thread.
        void Update();

        /// Draws the frame that was just updated.
        void Draw();


        /// Determines whether or not all of the keys in the given key combination are down.
        bool IsKeyCombinationDown(const KeyCombination &combination) const;


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
        void HandleEvent_OnMouseMove(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnMouseWheel(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnMouseButtonDown(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnMouseButtonUp(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnMouseButtonDoubleClick(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnKeyPressed(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnKeyReleased(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnKeyDown(GameEvent &e, GUIEventContext eventContext);
        void HandleEvent_OnKeyUp(GameEvent &e, GUIEventContext eventContext);
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

        /// Helper for posting the OnPause event to the scripting environment.
        void PostScriptEvent_OnPause();

        /// Helper for posting the OnResume event to the scripting environment.
        void PostScriptEvent_OnResume();


    private:

        /// The command line object.
        dr_cmdline m_cmdline;

        /// The absolute path of the executable.
        char m_executableAbsolutePath[DRFS_MAX_PATH];

        /// The absolute path of the directory of the application's executable.
        char m_executableDirectoryAbsolutePath[DRFS_MAX_PATH];


        /// A pointer to the object representing the virtual file system. This is where base directories are added.
        drfs_context* m_pVFS;

        /// The log file.
        drfs_file* m_pLogFile;


        /// A pointer to the dr_audio context for audio playback.
        dra_context* m_pAudioContext;

        /// A pointer to the device for audio playback.
        dra_device* m_pAudioPlaybackDevice;

        /// The global sound world.
        GT::SoundWorld m_soundWorld;


        /// The asset library.
        AssetLibrary m_assetLibrary;


        /// The script library.
        ScriptLibrary m_scriptLibrary;

        /// The particle system library.
        ParticleSystemLibrary m_particleSystemLibrary;

        /// The prefab library.
        PrefabLibrary m_prefabLibrary;

        /// The model library.
        ModelLibrary m_modelLibrary;

        /// The material library.
        MaterialLibrary m_materialLibrary;

        /// The shader library.
        ShaderLibrary m_shaderLibrary;

        /// The vertex array library.
        VertexArrayLibrary m_vertexArrayLibrary;

        /// The texture library.
        Texture2DLibrary m_textureLibrary;



        /// The game state manager.
        GameStateManager &m_gameStateManager;


        /// Keeps track of whether or not the game is initialised.
        bool isInitialised;

        /// Keeps track of whether or not the game is closing.
        bool closing;

        
        /// The list of events that are queued and ready for processing.
        GameEventQueue eventQueue;

        /// The mutex for protecting access to the event queue.
        dr_mutex eventQueueLock;
        
        /// A pointer to the event filter to filter events with. This can be null, in which case events will be dispatched without filtering. Defaults to null.
        GameEventFilter* eventFilter;

        /// The game's window.
        Window* window;

        /// The event handler for the main window.
        GameWindowEventHandler windowEventHandler;


        /// The scripting environment for doing anything with scripts.
        mutable GameScript script;


        /// The time between the last two frames. Use this for time-based operations.
        double deltaTimeInSeconds;

        /// The amount of time the game has been running, not including pause time.
        double totalRunninTimeInSeconds;


        /// The timer for timing updates. This is needed for retrieving the delta time.
        Timer updateTimer;
        

        /// The GUI image manager.
        DefaultGUIImageManager guiImageManager;

        /// The GUI of this game. There is only a single GUI for every game. A game should dynamically show and hide root elements
        /// to show different GUI's for different game states.
        GUIServer gui;

        /// The event handler for the GUI.
        GUIEventHandler guiEventHandler;

        /// The GUI renderer.
        DefaultGUIRenderer guiRenderer;

        /// The GUI element for the main game window.
        GUIElement* gameWindowGUIElement;


        /// Whether or not the game is paused. A paused game will continue to render, but scene nodes and animations will not tick.
        bool paused;

        /// Whether or not the game window has focus.
        bool focused;

        /// Whether or not the cursor is visible.
        bool isCursorVisible;

        /// Keeps track of whether or not automatic script reloading is enabled. False by default.
        bool isAutoScriptReloadEnabled;


        /// A map of key states. This is modified as keys are pushed and released.
        Map<char32_t, bool> keyDownMap;

        /// A map containing mouse button states. This is modified as mouse buttons are pushed and released. These are index by
        /// the MouseButton enum.
        Map<MouseButton, bool> mouseButtonDownMap;


        /// Class representing the editor.
        Editor editor;


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



        /// The key binding for toggling the profiler.
        Key profilerToggleKey;


        /// The key combination for toggling the editor.
        KeyCombination editorToggleKeyCombination;



    private:    // No copying.
        Context(const Context &);
        Context & operator=(const Context &);
    };
}

#endif
