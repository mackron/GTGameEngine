// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/DefaultGameState.hpp>

namespace GT
{
    DefaultGameState::DefaultGameState()
        : GameState(),
          m_engine(), m_game(m_engine, *this),
          m_config(),
          m_guiRendererAndResourceManager(), m_guiFontManager(FontOption_NoClearType), m_gui(&m_guiRendererAndResourceManager, &m_guiFontManager, &m_guiRendererAndResourceManager),
          m_graphicsWorld(m_gui), m_graphicsAssetResourceManager(m_graphicsWorld, m_engine.GetAssetLibrary()),
          m_sceneCallback(), m_scene(m_sceneCallback),
          m_flags(0)
    {
    }

    DefaultGameState::~DefaultGameState()
    {
        this->Shutdown();
    }



    bool DefaultGameState::Startup(int argc, char** argv)
    {
        if (m_engine.Startup(argc, argv))
        {
            if (m_game.Startup())
            {
                return true;
            }
            else
            {
                // Failed to initialize the game. We want to shut down the engine and fall through to return false.
                m_engine.Shutdown();
            }
        }

        return false;
    }

    void DefaultGameState::Shutdown()
    {
        m_graphicsWorld.Shutdown();


        // Shut down the game before the engine.
        m_game.Shutdown();

        // Shut down the engine last to ensure that anything that depends on the engine has been shut down beforehand.
        m_engine.Shutdown();
    }

    int DefaultGameState::Run()
    {
        return m_game.Run();
    }


    HWindow DefaultGameState::GetPrimaryWindow() const
    {
        if (m_windows.GetCount() > 0)
        {
            return m_windows[0].hWindow;
        }

        return NULL;
    }

    HWindow DefaultGameState::CreateSecondaryWindow(int monitor, int xPos, int yPos, unsigned int width, unsigned int height, bool fullscreen)
    {
        return this->CreateGameWindow("", monitor, xPos, yPos, width, height, fullscreen);
    }



    void DefaultGameState::EnableEditorShortcut()
    {
        m_flags &= ~Flag_IsEditorShortcutDisabled;
    }

    void DefaultGameState::DisableEditorShortcut()
    {
        m_flags |= Flag_IsEditorShortcutDisabled;
    }

    bool DefaultGameState::IsEditorShortcutEnabled() const
    {
        return (m_flags & Flag_IsEditorShortcutDisabled) == 0;
    }




    ///////////////////////////////////////////
    // Virtual Methods

    bool DefaultGameState::OnStartup(const GTLib::CommandLine &)
    {
        return true;
    }

    void DefaultGameState::OnShutdown()
    {
    }

    void DefaultGameState::OnStep(double)
    {
    }

    void DefaultGameState::OnStepWhilePaused(double)
    {
    }

    void DefaultGameState::OnPause()
    {
    }

    void DefaultGameState::OnResume()
    {
    }

    void DefaultGameState::OnMouseMove(HWindow, int, int)
    {
    }

    void DefaultGameState::OnMouseEnter(HWindow)
    {
    }

    void DefaultGameState::OnMouseLeave(HWindow)
    {
    }

    void DefaultGameState::OnMouseButtonPressed(HWindow, int, int, int)
    {
    }

    void DefaultGameState::OnMouseButtonReleased(HWindow, int, int, int)
    {
    }

    void DefaultGameState::OnMouseButtonDoubleClicked(HWindow, int, int, int)
    {
    }

    void DefaultGameState::OnMouseWheel(HWindow, int, int, int)
    {
    }

    void DefaultGameState::OnKeyPressed(HWindow, GTLib::Key)
    {
    }

    void DefaultGameState::OnKeyReleased(HWindow, GTLib::Key)
    {
    }

    void DefaultGameState::OnWindowResized(HWindow, unsigned int, unsigned int)
    {
    }





    ///////////////////////////////////////////
    // Inherited from GT::GameState

    unsigned int DefaultGameState::GetWindowCount(GT::GameContext &) const
    {
        return static_cast<unsigned int>(m_windows.GetCount());
    }

    HWindow DefaultGameState::GetWindowByIndex(GT::GameContext &, unsigned int index)
    {
        assert(index < m_windows.GetCount());
        return m_windows[index].hWindow;
    }

    void DefaultGameState::Step(GT::GameContext &, double deltaTimeInSeconds)
    {
        if (!m_game.IsPaused())
        {
            // Step the scene before calling OnStep()
            m_scene.Step(deltaTimeInSeconds);
            this->OnStep(deltaTimeInSeconds);
        }
        else
        {
            this->OnStepWhilePaused(deltaTimeInSeconds);
        }


        // Render the world after updating.
        m_graphicsWorld.ExecuteRenderingCommands();
    }

    bool DefaultGameState::OnStarting(GT::GameContext &, const GTLib::CommandLine &commandLine)
    {
        // Load the config before anything else. It doesn't matter if this fails because we will just use default in that case.
        this->LoadConfig(commandLine);

        // Initialize the graphics sub-system before creating the window. We do it like this so we can have access to an initialized
        // graphics world which is where we'll create the render target for the primary window.
        if (!this->InitializeGraphics())
        {
            return false;
        }

        // We want to create and display the window as soon as possible so that the user is given feedback that the game is starting. In
        // this case it is immediately after initializing the graphics system.
        if (!this->CreatePrimaryWindow())
        {
            return false;
        }


        // Make sure the GUI's DPI settings are correct.
        unsigned int baseDPIX;
        unsigned int baseDPIY;
        m_game.GetWindowManager().GetBaseDPI(baseDPIX, baseDPIY);
        m_gui.SetBaseDPI(baseDPIX, baseDPIY);


        return this->OnStartup(commandLine);
    }

    void DefaultGameState::OnClosing(GT::GameContext &)
    {
        this->OnShutdown();

        // Delete every window after OnShutdown().
        this->DeleteAllGameWindows();

        // Shutdown the graphics system after deleting all of the game windows.
        m_graphicsAssetResourceManager.Shutdown();
        m_graphicsWorld.Shutdown();
    }

    void DefaultGameState::OnPause(GT::GameContext &)
    {
        this->OnPause();
    }

    void DefaultGameState::OnResume(GT::GameContext &)
    {
        this->OnResume();
    }

    void DefaultGameState::OnMouseMove(GT::GameContext &, GT::HWindow hWindow, int xPos, int yPos)
    {
        this->OnMouseMove(hWindow, xPos, yPos);
    }

    void DefaultGameState::OnMouseEnter(GT::GameContext &, GT::HWindow hWindow)
    {
        this->OnMouseEnter(hWindow);
    }

    void DefaultGameState::OnMouseLeave(GT::GameContext &, GT::HWindow hWindow)
    {
        this->OnMouseLeave(hWindow);
    }

    void DefaultGameState::OnMouseButtonPressed(GT::GameContext &, GT::HWindow hWindow, int button, int xPos, int yPos)
    {
        this->OnMouseButtonPressed(hWindow, button, xPos, yPos);
    }

    void DefaultGameState::OnMouseButtonReleased(GT::GameContext &, GT::HWindow hWindow, int button, int xPos, int yPos)
    {
        this->OnMouseButtonReleased(hWindow, button, xPos, yPos);
    }

    void DefaultGameState::OnMouseButtonDoubleClicked(GT::GameContext &, GT::HWindow hWindow, int button, int xPos, int yPos)
    {
        this->OnMouseButtonDoubleClicked(hWindow, button, xPos, yPos);
    }

    void DefaultGameState::OnMouseWheel(GT::GameContext &, GT::HWindow hWindow, int delta, int xPos, int yPos)
    {
        this->OnMouseWheel(hWindow, delta, xPos, yPos);
    }

    void DefaultGameState::OnKeyPressed(GT::GameContext &, GT::HWindow hWindow, GTLib::Key key)
    {
        if (this->IsEditorShortcutEnabled())
        {
            if (key == GTLib::Keys::Shift)
            {
                m_flags |= Flag_IsShiftKeyDown;
            }

            if (key == GTLib::Keys::Tab && (m_flags & Flag_IsShiftKeyDown) != 0)
            {
                m_game.ToggleEditor();
                return;     // Return here - don't want the tab key to be passed to the game because have just toggled the editor.
            }
        }

        this->OnKeyPressed(hWindow, key);
    }

    void DefaultGameState::OnKeyReleased(GT::GameContext &, GT::HWindow hWindow, GTLib::Key key)
    {
        if (this->IsEditorShortcutEnabled())
        {
            if (key == GTLib::Keys::Shift)
            {
                m_flags &= ~Flag_IsShiftKeyDown;;
            }
        }

        this->OnKeyReleased(hWindow, key);
    }

    void DefaultGameState::OnWindowResized(GT::GameContext &, GT::HWindow hWindow, unsigned int width, unsigned int height)
    {
        this->OnWindowResized(hWindow, width, height);
    }




    //////////////////////////////////////////
    // Private

    void DefaultGameState::LoadConfig(const GTLib::CommandLine &commandLine)
    {
        if (m_config.Initialize())
        {
            // The first, and lowest-priority config file should be default one - config.ini. It doesn't matter if this fails to load because
            // it will just fall back to defaults.
            m_config.LoadFile("config.ini", m_game.GetEngineContext().GetFileSystem());


            // If the application was a passed a "--config" variable, that needs to be loaded.
            const char** ppConfigPathFromCmdLine = commandLine.GetArgument("config");
            if (ppConfigPathFromCmdLine != nullptr)
            {
                while (ppConfigPathFromCmdLine[0] != nullptr)
                {
                    m_config.LoadFile(ppConfigPathFromCmdLine[0], m_game.GetEngineContext().GetFileSystem());
                    ppConfigPathFromCmdLine += 1;
                }
            }
        }
    }

    bool DefaultGameState::InitializeGraphics()
    {
        if (m_graphicsWorld.Startup(m_engine))
        {
            return m_graphicsAssetResourceManager.Startup();
        }

        return false;
    }

    bool DefaultGameState::CreatePrimaryWindow()
    {
        // Read the config.
        char title[256];
        unsigned int resolutionX  = 1280;
        unsigned int resolutionY  = 720;
        bool fullscreen = false;
        int monitor = GT_PRIMARY_MONITOR;

        strcpy_s(title, 256, "GTGameEngine");
        if (m_config.IsString("Title")) {
            m_config.GetString("Title", title, sizeof(title));
        }
        if (m_config.IsNumber("ResolutionX")) {
            resolutionX = static_cast<unsigned int>(m_config.GetInteger("ResolutionX"));
        }
        if (m_config.IsNumber("ResolutionY")) {
            resolutionY = static_cast<unsigned int>(m_config.GetInteger("ResolutionY"));
        }
        if (m_config.IsBoolean("Fullscreen")) {
            fullscreen = m_config.GetBoolean("Fullscreen");
        }
        if (m_config.IsNumber("Monitor")) {
            monitor = m_config.GetInteger("Monitor");
        }

        return this->CreateGameWindow(title, monitor, 0, 0, resolutionX, resolutionY, fullscreen) != NULL;
    }

    HWindow DefaultGameState::CreateGameWindow(const char* title, int monitor, int xPos, int yPos, unsigned int width, unsigned int height, bool fullscreen)
    {
        GT::WindowManager &windowManager = m_game.GetWindowManager();

        if (width == height && width == 0)
        {
            // Make the resolution the resolution of the specified monitor. We first try to get the resolution of the requested monitor, but if that fails we try the default monitor.
            if (!windowManager.GetMonitorResolution(monitor, width, height))
            {
                // We failed to retrieve the resolution of the requested monitor, so switch to the primary monitor.
                if (monitor != GT_PRIMARY_MONITOR && !windowManager.GetMonitorResolution(0, width, height))
                {
                    // We have failed to retrieve the resolution of the primary monitor. Switch to windowed mode with a resolution of 1280 x 720.
                    width  = 1280;
                    height = 720;
                    fullscreen = false;
                }

                monitor = GT_PRIMARY_MONITOR;
            }
        }

        GT::WindowType windowType = GT::WindowType::PrimaryWindow;
        if (fullscreen)
        {
            windowType = GT::WindowType::PrimaryWindow_Borderless;
        }


        int monitorOriginX = 0;
        int monitorOriginY = 0;
        windowManager.GetMonitorOrigin(monitor, monitorOriginX, monitorOriginY);

        WindowData window;
        window.hWindow = m_game.CreateMainWindow(windowType, title, monitorOriginX + xPos, monitorOriginY + yPos, width, height);
        if (window.hWindow == 0)
        {
            return false;
        }

        // Show the window ASAP.
        m_game.ShowWindow(window.hWindow);

        // We need to create a render 
        window.hRT = m_graphicsWorld.CreateRenderTargetFromWindow(reinterpret_cast<HWND>(window.hWindow), 0);

        // We need a GUI surface for the main window.
        window.hGUISurface = m_gui.CreateSurface();
        m_graphicsWorld.SetRenderTargetGUISurface(window.hRT, window.hGUISurface);
        m_graphicsWorld.SetRenderTargetSurfaceSizeToRenderTarget(window.hRT);


        // The surface's DPI needs to be set based on the DPI supplied by the operating system.
        unsigned int dpiX;
        unsigned int dpiY;
        if (windowManager.GetWindowDPI(window.hWindow, dpiX, dpiY))
        {
            m_gui.SetDPI(window.hGUISurface, dpiX, dpiY);
        }


        m_windows.PushBack(window);

        return window.hWindow;
    }

    void DefaultGameState::DeleteAllGameWindows()
    {
        for (size_t iWindow = 0; iWindow < m_windows.GetCount(); ++iWindow)
        {
            WindowData &window = m_windows[iWindow];

            m_graphicsWorld.DeleteRenderTarget(window.hRT);
            m_gui.DeleteSurface(window.hGUISurface);
            m_game.DeleteWindow(window.hWindow);
        }

        m_windows.Clear();
    }
}