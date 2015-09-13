// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameState.hpp>
#include <GTLib/Timing/TimingCommon.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTGameEngine/WindowManager_DefaultWin32.hpp>
#endif


namespace GT
{
    GameContext::GameContext(EngineContext &engineContext, GameState &gameState)
        : m_engineContext(engineContext),
          m_gameState(gameState),
          m_pWindowManager(nullptr),
          m_gui(),
          m_flags(0)
#if defined(GT_BUILD_EDITOR)
        , m_editor(*this),
          m_editorEventHandler(*this),
          m_wasCursorVisibleBeforeOpeningEditor(true)
#endif
    {
        m_flags |= StateFlag_IsRunningFlag;
        m_flags |= StateFlag_IsRunningRealTimeLoopFlag;

        m_flags |= StateFlag_IsSingleThreadedFlag;    // TODO: Remove this flag so that multi-threading is used by default.

        if (m_pWindowManager == nullptr)
        {
#if defined (GT_PLATFORM_WINDOWS)
            m_pWindowManager = new WindowManager_DefaultWin32(this);
#endif
#if defined (GT_PLATFORM_LINUX)
            m_pWindowManager = new WindowManager_DefaultX11(this);
#endif

            m_flags |= StateFlag_IsOwnerOfWindowManagerFlag;
        }
    }

    GameContext::~GameContext()
    {
        // Window manager.
        if ((m_flags & StateFlag_IsOwnerOfWindowManagerFlag) != 0)
        {
            delete m_pWindowManager;
        }
    }



    bool GameContext::Startup()
    {
        return true;
    }

    void GameContext::Shutdown()
    {
        // Some shutdown routines are done at at the end of Run(), such as the editor.

        // Window manager.
        if ((m_flags & StateFlag_IsOwnerOfWindowManagerFlag) != 0)
        {
            delete m_pWindowManager;
            m_pWindowManager = nullptr;
        }
    }


    EngineContext & GameContext::GetEngineContext()
    {
        return m_engineContext;
    }

    GameState & GameContext::GetGameState()
    {
        return m_gameState;
    }

    WindowManager & GameContext::GetWindowManager()
    {
        return *m_pWindowManager;
    }


    int GameContext::Run()
    {
        //////////////////////////////////////
        // Initialize.
        if (!m_gameState.OnStarting(*this, m_engineContext.GetCommandLine()))
        {
            return 1;
        }


        // --editor <file name>
        const char** ppEditor = m_engineContext.GetCommandLine().GetArgument("editor");
        if (ppEditor != nullptr) {
            this->OpenEditor();

            while (ppEditor[0] != nullptr)
            {
                char absoluteFilePath[GT_MAX_PATH];
                if (m_engineContext.GetFileSystem().FindAbsolutePath(ppEditor[0], absoluteFilePath, GT_MAX_PATH))
                {
                    m_editor.OpenFile(absoluteFilePath);
                }

                ppEditor += 1;
            }
        }


        // Set the initial frame time.
        m_lastFrameTime = GTLib::Timing::GetTimeInSeconds();


        //////////////////////////////////////
        // Loop. Not too complex...
        while ((m_flags & StateFlag_IsRunningFlag) != 0)
        {
            if ((m_flags & StateFlag_IsRunningRealTimeLoopFlag) != 0)
            {
                m_pWindowManager->RealTimeLoop([&]() -> bool
                {
                    if (((m_flags & StateFlag_IsRunningFlag) != 0) && ((m_flags & StateFlag_IsRunningRealTimeLoopFlag) != 0))
                    {
                        this->Step();
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                });

                // If we're still running a real-time loop at this point it means we have received a quit message and need to close.
                if ((m_flags & StateFlag_IsRunningRealTimeLoopFlag) != 0)
                {
                    m_flags &= ~StateFlag_IsRunningFlag;
                }
            }
            else
            {
                m_pWindowManager->EventDrivenLoop([&]() -> bool
                {
                    return (m_flags & StateFlag_IsRunningRealTimeLoopFlag) == 0;
                });

                // If we're still running at event-driven loop at this point it means we have received a quit message and need to close.
                if ((m_flags & StateFlag_IsRunningRealTimeLoopFlag) == 0)
                {
                    m_flags &= ~StateFlag_IsRunningFlag;
                }
            }
        }



        //////////////////////////////////////
        // Shutdown.

        // The editor needs to be shut down before the game state receives OnClosing() because the game state will destroy the window that the
        // editor uses as the parent of some of the editor's controls - these controls need to be destroyed first in order to clean them up properly.
#if defined(GT_BUILD_EDITOR)
        m_editor.Shutdown();
#endif

        // Post OnClosing to the game state after shutting down the editor.
        m_gameState.OnClosing(*this);


        return m_pWindowManager->GetExitCode();
    }


    unsigned int GameContext::GetWindowCount()
    {
        return m_gameState.GetWindowCount(*this);
    }

    HWindow GameContext::GetWindowByIndex(unsigned int index)
    {
        return m_gameState.GetWindowByIndex(*this, index);
    }


    bool GameContext::GetWindowSize(HWindow hWindow, unsigned int &widthOut, unsigned int &heightOut) const
    {
        if (m_pWindowManager != nullptr)
        {
            return m_pWindowManager->GetWindowSize(hWindow, widthOut, heightOut);
        }

        return false;
    }


    HWindow GameContext::CreateMainWindow(WindowType type, const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_pWindowManager->CreateWindow(0, type, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_pWindowManager->SetWindowTitle(hWindow, title);
        }

        return hWindow;
    }

    HWindow GameContext::CreateChildWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        return m_pWindowManager->CreateWindow(hParent, WindowType::ChildWindow, xPos, yPos, width, height);
    }

    HWindow GameContext::CreatePopupWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        return m_pWindowManager->CreateWindow(hParent, WindowType::PopupWindow, xPos, yPos, width, height);
    }

    void GameContext::DeleteWindow(HWindow hWindow)
    {
        m_pWindowManager->DeleteWindow(hWindow);
    }


    void GameContext::ShowWindow(HWindow hWindow)
    {
        m_pWindowManager->ShowWindow(hWindow);
    }

    void GameContext::HideWindow(HWindow hWindow)
    {
        m_pWindowManager->HideWindow(hWindow);
    }



    void GameContext::Close()
    {
        m_pWindowManager->PostQuitMessage(0);
    }

    void GameContext::Step()
    {
        if (!this->IsEditorOpen())
        {
            double currentTime = GTLib::Timing::GetTimeInSeconds();
            double deltaTimeInSeconds = currentTime - m_lastFrameTime;
            {
#if defined(GT_BUILD_EDITOR)
                if (!this->IsEditorOpen())
                {
                    m_gameState.Step(*this, deltaTimeInSeconds);
                }
                else
                {
                    m_editor.Step(deltaTimeInSeconds);
                }
#else
                m_gameState.Step(*this, deltaTimeInSeconds);
#endif
            }
            m_lastFrameTime = currentTime;
        }
    }

    void GameContext::Pause()
    {
        if (!this->IsEditorOpen())
        {
            if (!this->IsPaused())
            {
                m_flags |= StateFlag_IsPaused;
                m_gameState.OnPause(*this);
            }
        }
    }

    void GameContext::Resume()
    {
        if (!this->IsEditorOpen())
        {
            if (this->IsPaused())
            {
                m_flags &= ~StateFlag_IsPaused;
                m_gameState.OnResume(*this);
            }
        }
    }

    bool GameContext::IsPaused() const
    {
        return (m_flags & StateFlag_IsPaused) != 0;
    }


    GUIContext & GameContext::GetGUI()
    {
        return m_gui;
    }


    bool GameContext::IsKeyDown(GTLib::Key key)
    {
        return m_pWindowManager->IsKeyDown(key);
    }


    bool GameContext::GetCursorPosition(HWindow hWindow, int &mousePosXOut, int &mousePosYOut) const
    {
        return m_pWindowManager->GetCursorPosition(hWindow, mousePosXOut, mousePosYOut);
    }

    bool GameContext::IsMouseInsideWindow(HWindow hWindow) const
    {
        int mousePosX;
        int mousePosY;
        if (this->GetCursorPosition(hWindow, mousePosX, mousePosY))
        {
            if (mousePosX >= 0 && mousePosY >= 0)
            {
                unsigned int windowWidth;
                unsigned int windowHeight;
                if (this->GetWindowSize(hWindow, windowWidth, windowHeight))
                {
                    return static_cast<unsigned int>(mousePosX) < windowWidth && static_cast<unsigned int>(mousePosY) < windowHeight;
                }
            }
        }

        return false;
    }

    
    void GameContext::ShowCursor()
    {
        m_pWindowManager->ShowCursor();
    }

    void GameContext::HideCursor()
    {
        m_pWindowManager->HideCursor();
    }

    bool GameContext::IsCursorVisible() const
    {
        return m_pWindowManager->IsCursorVisible();
    }


    bool GameContext::OpenEditor()
    {
#if defined(GT_BUILD_EDITOR)
        if (!this->IsEditorOpen())
        {
            // Startup the editor if it hasn't already.
            if ((m_flags & StateFlag_IsEditorInitialisedFlag) == 0)
            {
                if (m_editor.Startup())
                {
                    m_editor.AttachEventHandler(m_editorEventHandler);
                    m_flags |= StateFlag_IsEditorInitialisedFlag;
                }
                else
                {
                    // Failed to start up the editor.
                    return false;
                }
            }

            m_wasCursorVisibleBeforeOpeningEditor = this->IsCursorVisible();

            return m_editor.Open();     // This will post OnEditorOpened()
        }
        else
        {
            // Editor is already open.
            return true;
        }
#else
        return false;
#endif
    }

    void GameContext::CloseEditor()
    {
#if defined(GT_BUILD_EDITOR)
        m_editor.Close();       // This will post OnEditorClosed()

        if (m_wasCursorVisibleBeforeOpeningEditor) {
            this->ShowCursor();
        } else {
            this->HideCursor();
        }
#endif
    }

    bool GameContext::IsEditorOpen() const
    {
#if defined(GT_BUILD_EDITOR)
        return m_editor.IsOpen();
#else
        return false;
#endif
    }

    bool GameContext::ToggleEditor()
    {
#if defined(GT_BUILD_EDITOR)
        if (this->IsEditorOpen())
        {
            this->CloseEditor();
            return true;
        }
        else
        {
            return this->OpenEditor();
        }
#else
        return false;
#endif
    }



    void GameContext::OnWantToClose()
    {
        if (!this->IsEditorOpen())
        {
            if (m_gameState.OnWantToClose(*this))
            {
                this->Close();
            }
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            if (m_editor.OnWantToClose())
            {
                this->Close();
            }
        }
#endif
    }

    void GameContext::OnWindowActivated(HWindow hWindow)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnWindowActivated(*this, hWindow);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnWindowActivated(hWindow);
        }
#endif
    }

    void GameContext::OnWindowDeactivated(HWindow hWindow)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnWindowDeactivated(*this, hWindow);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnWindowDeactivated(hWindow);
        }
#endif
    }

    void GameContext::OnWindowCreated(HWindow hWindow)
    {
        m_gameState.OnWindowCreated(*this, hWindow);

#if defined(GT_BUILD_EDITOR)
        m_editor.OnWindowCreated(hWindow);
#endif
    }

    void GameContext::OnWindowDeleting(HWindow hWindow)
    {
        m_gameState.OnWindowDeleting(*this, hWindow);

#if defined(GT_BUILD_EDITOR)
        m_editor.OnWindowDeleting(hWindow);
#endif
    }

    void GameContext::OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnWindowResized(*this, hWindow, width, height);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnWindowResized(hWindow, width, height);
        }
#endif
    }

    void GameContext::OnWindowMoved(HWindow hWindow, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnWindowMoved(*this, hWindow, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnWindowMoved(hWindow, xPos, yPos);
        }
#endif
    }

    void GameContext::OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseMove(*this, hWindow, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseMove(hWindow, xPos, yPos);
        }
#endif
    }

    void GameContext::OnMouseEnter(HWindow hWindow)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseEnter(*this, hWindow);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseEnter(hWindow);
        }
#endif
    }

    void GameContext::OnMouseLeave(HWindow hWindow)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseLeave(*this, hWindow);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseLeave(hWindow);
        }
#endif
    }

    void GameContext::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseButtonPressed(*this, hWindow, button, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseButtonPressed(hWindow, button, xPos, yPos);
        }
#endif
    }

    void GameContext::OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseButtonReleased(*this, hWindow, button, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseButtonReleased(hWindow, button, xPos, yPos);
        }
#endif
    }

    void GameContext::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseButtonDoubleClicked(*this, hWindow, button, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseButtonDoubleClicked(hWindow, button, xPos, yPos);
        }
#endif
    }

    void GameContext::OnMouseWheel(HWindow hWindow, int delta, int xPos, int yPos)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnMouseWheel(*this, hWindow, delta, xPos, yPos);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnMouseWheel(hWindow, delta, xPos, yPos);
        }
#endif
    }

    void GameContext::OnKeyPressed(HWindow hWindow, GTLib::Key key)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnKeyPressed(*this, hWindow, key);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnKeyPressed(hWindow, key);
        }
#endif
    }

    void GameContext::OnKeyPressedAutoRepeat(HWindow hWindow, GTLib::Key key)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnKeyPressedAutoRepeat(*this, hWindow, key);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnKeyPressedAutoRepeat(hWindow, key);
        }
#endif
    }

    void GameContext::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnKeyReleased(*this, hWindow, key);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnKeyReleased(hWindow, key);
        }
#endif
    }

    void GameContext::OnPrintableKeyDown(HWindow hWindow, char32_t character)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnPrintableKeyDown(*this, hWindow, character);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnPrintableKeyDown(hWindow, character);
        }
#endif
    }

    void GameContext::OnPaintWindow(HWindow hWindow, const GTLib::Rect<int> &rect)
    {
        if (!this->IsEditorOpen())
        {
            m_gameState.OnPaintWindow(*this, hWindow, rect);
        }
#if defined(GT_BUILD_EDITOR)
        else
        {
            m_editor.OnPaintWindow(hWindow, rect);
        }
#endif
    }


    void GameContext::OnEditorOpened()
    {
#if defined(GT_BUILD_EDITOR)
        m_flags &= ~StateFlag_IsRunningRealTimeLoopFlag;      //< Switch to an event-driven application loop.
#endif
    }

    void GameContext::OnEditorClosed()
    {
#if defined(GT_BUILD_EDITOR)
        m_flags |= StateFlag_IsRunningRealTimeLoopFlag;       //< Switch to a real-time game loop.
#endif
    }
}
