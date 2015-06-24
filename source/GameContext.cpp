// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameState.hpp>
#include <GTGameEngine/GameDisplay_Windowed.hpp>
#include <GTGameEngine/GameDisplayRenderingCallback.hpp>
#include <GTGameEngine/GPURenderingDevice.hpp>
#include <GTLib/Timing/TimingCommon.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include "GameContext_Win32.hpp"
#include <GTGameEngine/WindowManager_DefaultWin32.hpp>
#endif


namespace GT
{
    GameContext::GameContext(EngineContext &engineContext, GameState &gameState)
        : m_engineContext(engineContext),
          m_gameState(gameState),
          m_pWindowManager(nullptr),
          m_windowedDisplays(),
          m_flags(0)
#if defined(GT_BUILD_EDITOR)
        , m_editor(*this),
          m_editorEventHandler(*this)
#endif
    {
        m_flags |= IsRunningFlag;
        m_flags |= IsRunningRealTimeLoopFlag;

        m_flags |= IsSingleThreadedFlag;    // TODO: Remove this flag so that multi-threading is used by default.

        if (m_pWindowManager == nullptr)
        {
#if defined (GT_PLATFORM_WINDOWS)
            m_pWindowManager = new WindowManager_DefaultWin32(this);
#endif
#if defined (GT_PLATFORM_LINUX)
            m_pWindowManager = new WindowManager_DefaultX11(this);
#endif

            m_flags |= IsOwnerOfWindowManagerFlag;
        }
    }

    GameContext::~GameContext()
    {
        // Window manager.
        if ((m_flags & IsOwnerOfWindowManagerFlag) != 0)
        {
            delete m_pWindowManager;
        }
    }



    ResultCode GameContext::Startup()
    {
        return 0;   // No error.
    }

    void GameContext::Shutdown()
    {
        // Window manager.
        if ((m_flags & IsOwnerOfWindowManagerFlag) != 0)
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


        // Set the initial frame time.
        m_lastFrameTime = GTLib::Timing::GetTimeInSeconds();


        //////////////////////////////////////
        // Loop. Not too complex...
        while ((m_flags & IsRunningFlag) != 0)
        {
            if ((m_flags & IsRunningRealTimeLoopFlag) != 0)
            {
                m_pWindowManager->RealTimeLoop([&]() -> bool
                {
                    if (((m_flags & IsRunningFlag) != 0) && ((m_flags & IsRunningRealTimeLoopFlag) != 0))
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
                if ((m_flags & IsRunningRealTimeLoopFlag) != 0)
                {
                    m_flags &= ~IsRunningFlag;
                }
            }
            else
            {
                m_pWindowManager->EventDrivenLoop([&]() -> bool
                {
                    return (m_flags & IsRunningRealTimeLoopFlag) == 0;
                });

                // If we're still running at event-driven loop at this point it means we have received a quit message and need to close.
                if ((m_flags & IsRunningRealTimeLoopFlag) == 0)
                {
                    m_flags &= ~IsRunningFlag;
                }
            }
        }


        //////////////////////////////////////
        // Shutdown.
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


    bool GameContext::OpenEditor()
    {
#if defined(GT_BUILD_EDITOR)
        // Startup the editor if it hasn't already.
        if ((m_flags & IsEditorInitialisedFlag) == 0)
        {
            if (m_editor.Startup())
            {
                m_editor.AttachEventHandler(m_editorEventHandler);
                m_flags |= IsEditorInitialisedFlag;
            }
            else
            {
                // Failed to start up the editor.
                return false;
            }
        }


        return m_editor.Open();     // This will post OnEditorOpened()
#else
        return false;
#endif
    }

    void GameContext::CloseEditor()
    {
#if defined(GT_BUILD_EDITOR)
        m_editor.Close();       // This will post OnEditorClosed()
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
        m_flags &= ~IsRunningRealTimeLoopFlag;      //< Switch to an event-driven application loop.
#endif
    }

    void GameContext::OnEditorClosed()
    {
#if defined(GT_BUILD_EDITOR)
        m_flags |= IsRunningRealTimeLoopFlag;       //< Switch to a real-time game loop.
#endif
    }



    //////////////////////////////////////////////////////////////////
    // Win32
#if defined(GT_PLATFORM_WINDOWS)
    GameDisplay* GameContext::CreateWindowedDisplay(GraphicsInterface &graphicsInterface, HWND hWnd)
    {
        GameDisplay* displayOut = nullptr;

        if (hWnd != 0)
        {
            ResultCode result = graphicsInterface.InitWindowBuffers(hWnd, true);
            if (GT::Succeeded(result))
            {
                auto display = new GameDisplay_Windowed(graphicsInterface, hWnd);
                    
                m_windowedDisplays.PushBack(display);

                displayOut = display;
            }
            else
            {
                //return result;
            }
        }
        else
        {
            //return InvalidWindow;
        }

        return displayOut;
    }

    GameDisplay* GameContext::GetWindowedDisplayFromWindow(HWND hWnd)
    {
        for (size_t iDisplay = 0; iDisplay < m_windowedDisplays.GetCount(); ++iDisplay)
        {
            if (m_windowedDisplays[iDisplay]->GetWindow() == hWnd)
            {
                return m_windowedDisplays[iDisplay];
            }
        }

        return nullptr;
    }

    void GameContext::DeleteDisplay(GameDisplay* display)
    {
        if (display != nullptr)
        {
            switch (display->GetType())
            {
            case GameDisplayType_Windowed:
                {
                    display->GetGraphicsInterface().UninitWindowBuffers(reinterpret_cast<GameDisplay_Windowed*>(display)->GetWindow());
                    break;
                }


            case GameDisplayType_Fullscreen:
            case GameDisplayType_Texture:
            default: break;
            }
        }

        delete display;
    }
#endif
}
