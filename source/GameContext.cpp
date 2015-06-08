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


    HWindow GameContext::CreateMainWindow(WindowType type, const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_pWindowManager->CreateWindow(0, type, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_pWindowManager->SetWindowTitle(hWindow, title);
            m_pWindowManager->ShowWindow(hWindow);
        }
     
        return hWindow;
    }

    HWindow GameContext::CreateChildWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_pWindowManager->CreateWindow(hParent, WindowType::ChildWindow, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_pWindowManager->ShowWindow(hWindow);
        }

        return hWindow;
    }

    HWindow GameContext::CreatePopupWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_pWindowManager->CreateWindow(hParent, WindowType::PopupWindow, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_pWindowManager->ShowWindow(hWindow);
        }

        return hWindow;
    }

    void GameContext::DeleteWindow(HWindow hWindow)
    {
        m_pWindowManager->DeleteWindow(hWindow);
    }



    void GameContext::Close()
    {
        m_pWindowManager->PostQuitMessage(0);
    }

    void GameContext::Step()
    {
        double currentTime = GTLib::Timing::GetTimeInSeconds();
        double deltaTimeInSeconds = currentTime - m_lastFrameTime;

        m_gameState.Step(deltaTimeInSeconds);

        m_lastFrameTime = currentTime;

        // The way the game is stepped depends on whether or not multithreading is being used. In some cases it is
        // desireable to not multi-thread things, such as when single-threading is efficient enough that it becomes
        // faster.
        //
        // In a single threaded environment it is very simple - we simply do an update tick, and then issue the
        // rendering commands directly.
        //
        // In a multi-threaded environment, we want to buffer the rendering commands, and then execute them on the
        // following frame.
        //
        // Because the multi-threaded implementation is less efficient we do not want to use a buffering system for
        // the single-threaded case. We do, however, need a consistent API for both since we need to allow custom
        // rendering for displays.

        if ((m_flags & IsSingleThreadedFlag) == 0)
        {
            // Multi-threaded pipeline.
        }
        else
        {
            // Single-threaded pipeline.

            // We need to iterate over each display and draw it.
            for (size_t iDisplay = 0; iDisplay < m_windowedDisplays.GetCount(); ++iDisplay)
            {
                GameDisplay_Windowed* display = m_windowedDisplays[iDisplay];
                assert(display != nullptr);
                {
                    GameDisplayRenderingCallback* renderingCallback = display->GetRenderingCallback();
                    if (renderingCallback != nullptr)
                    {
                        renderingCallback->Render(*display);
                    }
                }
            }
        }
    }


    void GameContext::OpenEditor()
    {
        if (!this->IsEditorOpen())
        {
            if (m_gameState.OnWantToOpenEditor(*this))
            {
                m_flags |= IsEditorOpenFlag;
                m_flags &= ~IsRunningRealTimeLoopFlag;  //< Switch to an event-driven application loop.
            }
        }
    }

    void GameContext::CloseEditor()
    {
        if (this->IsEditorOpen())
        {
            m_flags &= ~IsEditorOpenFlag;
            m_flags |= IsRunningRealTimeLoopFlag;       //< Switch to a real-time game loop.
        }
    }

    bool GameContext::IsEditorOpen() const
    {
        return (m_flags & IsEditorOpenFlag) != 0;
    }



    void GameContext::OnWantToClose()
    {
        if (m_gameState.OnWantToClose(*this))
        {
            this->Close();
        }
    }

    void GameContext::OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height)
    {
        m_gameState.OnWindowResized(*this, hWindow, width, height);
    }

    void GameContext::OnWindowMoved(HWindow hWindow, int xPos, int yPos)
    {
        m_gameState.OnWindowMoved(*this, hWindow, xPos, yPos);
    }

    void GameContext::OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
        m_gameState.OnMouseMove(*this, hWindow, xPos, yPos);
    }

    void GameContext::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
        m_gameState.OnMouseButtonPressed(*this, hWindow, button, xPos, yPos);
    }

    void GameContext::OnMouseButtonReleased(HWindow hWindow, int button, int xPos, int yPos)
    {
        m_gameState.OnMouseButtonReleased(*this, hWindow, button, xPos, yPos);
    }

    void GameContext::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos)
    {
        m_gameState.OnMouseButtonDoubleClicked(*this, hWindow, button, xPos, yPos);
    }

    void GameContext::OnKeyPressed(HWindow hWindow, GTLib::Key key)
    {
        m_gameState.OnKeyPressed(*this, hWindow, key);
    }

    void GameContext::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
        m_gameState.OnKeyReleased(*this, hWindow, key);
    }

    void GameContext::OnPrintableKeyDown(HWindow hWindow, char32_t character)
    {
        m_gameState.OnPrintableKeyDown(*this, hWindow, character);
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