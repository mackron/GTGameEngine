// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_DefaultGameState
#define GT_DefaultGameState

#include "GameState.hpp"
#include "EngineContext.hpp"
#include "GameContext.hpp"
#include "ConfigFile.hpp"
#include "Graphics/GraphicsWorldGUIRendererAndResourceManager.hpp"
#include "Graphics/DefaultGraphicsWorld.hpp"
#include "Scene/Scene.hpp"
#include "Scene/DefaultSceneCallback.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    /// Helper class for game states.
    ///
    /// This can be used as the basis for most games. This will create the engine and game context in Startup().
    class DefaultGameState : public GameState
    {
    public:

        /// Constructor.
        DefaultGameState();

        /// Destructor.
        virtual ~DefaultGameState();


        /// Starts up the game.
        ///
        /// @remarks
        ///     This will initialize the engine and game context's, load the config files and create the primary game window. Create
        ///     secondary game windows with CreateSecondaryGameWindow().
        bool Startup(int argc, char** argv);

        /// Shuts down the game.
        void Shutdown();

        /// Begins the game loop and returns the exit code.
        ///
        /// @remarks
        ///     Upon returning, this does NOT shut down the game with Shutdown() - that must be done manually, or left to the destructor.
        int Run();


        /// Retrieves a pointer to the engine context.
        EngineContext & GetEngineContext() { return m_engine; }

        /// Retrieves a reference to the game context.
        GameContext & GetGameContext() { return m_game; }

        /// Retrieves a reference to the object representing the config file.
        ConfigFile & GetConfig() { return m_config; }


        /// Retrieves a handle to the primary window.
        HWindow GetPrimaryWindow() const;

        /// Creates a secondary game window.
        ///
        /// @remarks
        ///     The primary window is created automatically by Startup().
        HWindow CreateSecondaryWindow(int monitor, int xPos, int yPos, unsigned int width, unsigned int height, bool fullscreen);


        /// Enables the editor toggle shortcut (enabled by default).
        void EnableEditorShortcut();

        /// Disables the editor toggle shortcut.
        void DisableEditorShortcut();

        /// Determines whether or not the editor shortcut is enabled.
        bool IsEditorShortcutEnabled() const;



        ///////////////////////////////////////////
        // Virtual Methods

        /// Called when the game is initializing.
        ///
        /// @remarks
        ///     This is where the game should do things like load the initial scene and whatnot.
        ///     @par
        ///     Return false if startup fails.
        virtual bool OnStartup(const GTLib::CommandLine &commandLine);

        /// Called when the game is closing and needs to be uninitialized.
        virtual void OnShutdown();

        /// Called when the game is being stepped/updated.
        ///
        /// @param dt [in] The time in seconds since the previous frame.
        virtual void OnStep(double dt);

        /// Called when the game is being stepped/updated while in a paused state.
        ///
        /// @param dt [in] The time in seconds since the previous frame.
        virtual void OnStepWhilePaused(double dt);

        /// Called after the game has been paused.
        virtual void OnPause();

        /// Called after the game has resumed after being paused.
        virtual void OnResume();

        /// Called when the mouse is moved while over the given window.
        virtual void OnMouseMove(HWindow hWindow, int xPos, int yPos);
        
        /// Called when the mouse enters the given window.
        virtual void OnMouseEnter(HWindow hWindow);
        
        /// Called when the mouse leaves the given window.
        virtual void OnMouseLeave(HWindow hWindow);
        
        /// Called when a mouse button is pressed while over the given window.
        virtual void OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos);
        
        /// Called when a mouse button is released while over the given window.
        virtual void OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos);
        
        /// Called when a mouse button is double-clicked while over the given window.
        virtual void OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos);
        
        /// Called when the mouse wheel is turned while over the given window.
        virtual void OnMouseWheel(HWindow hWindow, int delta, int xPos, int yPos);
        
        /// Called when a key is pressed while the given window has the keyboard focus.
        virtual void OnKeyPressed(HWindow hWindow, GTLib::Key key);
        
        /// Called when a key is released while the given window has the keyboard focus.
        virtual void OnKeyReleased(HWindow hWindow, GTLib::Key key);
        
        /// Called when the given window is resized.
        virtual void OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height);





        ///////////////////////////////////////////
        // Inherited from GameState

        /// @copydoc GameState::GetWindowCount()
        unsigned int GetWindowCount(GameContext &gameContext) const;

        /// @copydoc GameState::GetWindowByIndex()
        HWindow GetWindowByIndex(GameContext &gameContext, unsigned int index);

        /// @copydoc GameState::Step()
        void Step(GameContext &gameContext, double deltaTimeInSeconds);

        /// @copydoc GameState::OnStarting()
        bool OnStarting(GameContext &gameContext, const GTLib::CommandLine &commandLine);

        /// @copydoc GameState::OnClosing()
        void OnClosing(GameContext &gameContext);

        /// @copydoc GameState::OnPause()
        void OnPause(GT::GameContext &gameContext);
        
        /// @copydoc GameState::OnResume()
        void OnResume(GT::GameContext &gameContext);
        
        /// @copydoc GameState::OnMouseMove()
        void OnMouseMove(GT::GameContext &gameContext, GT::HWindow hWindow, int xPos, int yPos);
        
        /// @copydoc GameState::OnMouseEnter()
        void OnMouseEnter(GT::GameContext &gameContext, GT::HWindow hWindow);
        
        /// @copydoc GameState::OnMouseLeave()
        void OnMouseLeave(GT::GameContext &gameContext, GT::HWindow hWindow);
        
        /// @copydoc GameState::OnMouseButtonPressed()
        void OnMouseButtonPressed(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        
        /// @copydoc GameState::OnMouseButtonReleased()
        void OnMouseButtonReleased(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        
        /// @copydoc GameState::OnMouseButtonDoubleClicked()
        void OnMouseButtonDoubleClicked(GT::GameContext &gameContext, GT::HWindow hWindow, int button, int xPos, int yPos);
        
        /// @copydoc GameState::OnMouseWheel()
        void OnMouseWheel(GT::GameContext &gameContext, GT::HWindow hWindow, int delta, int xPos, int yPos);
        
        /// @copydoc GameState::OnKeyPressed()
        void OnKeyPressed(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key);
        
        /// @copydoc GameState::OnKeyReleased()
        void OnKeyReleased(GT::GameContext &gameContext, GT::HWindow hWindow, GTLib::Key key);
        
        /// @copydoc GameState::OnWindowResized()
        void OnWindowResized(GT::GameContext &gameContext, GT::HWindow hWindow, unsigned int width, unsigned int height);


    private:

        /// Helper for loading the config.
        void LoadConfig(const GTLib::CommandLine &commandLine);

        /// Initializes the graphics sub-system.
        bool InitializeGraphics();

        /// Creates the primary game window based on the config file.
        bool CreatePrimaryWindow();

        /// Helper for creating a primary or secondary window.
        HWindow CreateGameWindow(const char* title, int monitor, int xPos, int yPos, unsigned int width, unsigned int height, bool fullscreen);

        /// Helper for deleting every game window.
        void DeleteAllGameWindows();


    private:

        /// Data associated with each window.
        struct WindowData
        {
            /// The window.
            HWindow hWindow;
            
            /// The window's render target.
            HGraphicsRenderTarget hRT;

            /// The window's GUI surface.
            HGUISurface hGUISurface;
        };


        /// A reference to the engine context. This is initialized in Startup()
        EngineContext m_engine;

        /// A pointer to the game context. This is initialized in Startup()
        GameContext m_game;

        /// The config file. Config files are loaded by LoadConfig(), which is called by OnStarting()
        ConfigFile m_config; 


        /// The GUI renderer.
        GraphicsWorldGUIRendererAndResourceManager m_guiRendererAndResourceManager;

#if defined(GT_PLATFORM_WINDOWS)
        /// The GUI font manager. For now we're using GDI, but later on we'll make once specific to GraphicsWorld which will be better optimized.
        GUIFontManager_GDI m_guiFontManager;
#endif

        /// The GUI.
        GUIContext m_gui;


        /// The graphics world for drawing the current scene.
        DefaultGraphicsWorld m_graphicsWorld;

        /// The graphics asset resource manager.
        GraphicsAssetResourceManager m_graphicsAssetResourceManager;


        /// The scene callback.
        DefaultSceneCallback m_sceneCallback;

        /// The actual scene.
        Scene m_scene;


        /// The list of windows. The first window in this list is always the primary window.
        GTLib::Vector<WindowData> m_windows;


        /// Flags.
        uint32_t m_flags;


        ////////////////////////////////////////
        // Flags

        static const uint32_t Flag_IsEditorShortcutDisabled = (1 << 0);
        static const uint32_t Flag_IsShiftKeyDown           = (1 << 1);       // For opening the editor with Shift+Tab.


    private:    // No copying.
        DefaultGameState(const DefaultGameState &);
        DefaultGameState & operator=(const DefaultGameState &);
    };
}

#endif
