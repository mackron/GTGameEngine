// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/EngineContext.hpp>
#include <GTGameEngine/GameContext.hpp>
#include <GTGameEngine/GameDisplay_Windowed.hpp>
#include <GTGameEngine/GameDisplayRenderingCallback.hpp>
#include <GTGameEngine/GPURenderingDevice.hpp>

#if defined(GT_PLATFORM_WINDOWS)
#include "GameContext_Win32.hpp"
#endif

namespace GT
{
    GameContext::GameContext(EngineContext &engineContext)
        : m_engineContext(engineContext),
            m_windowedDisplays(),
            m_flags()
    {
        m_flags |= IsSingleThreaded;    // TODO: Remove this flag so that multi-threading is used by default.
    }

    GameContext::~GameContext()
    {
    }


    ResultCode GameContext::Startup()
    {
        return 0;   // No error.
    }

    void GameContext::Shutdown()
    {
    }



    HWindow GameContext::CreateMainWindow(WindowType type, const char* title, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_engineContext.GetWindowManager().CreateWindow(0, type, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_engineContext.GetWindowManager().SetWindowTitle(hWindow, title);
            m_engineContext.GetWindowManager().ShowWindow(hWindow);
        }
     
        return hWindow;
    }

    HWindow GameContext::CreateChildWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_engineContext.GetWindowManager().CreateWindow(hParent, WindowType::ChildWindow, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_engineContext.GetWindowManager().ShowWindow(hWindow);
        }

        return hWindow;
    }

    HWindow GameContext::CreatePopupWindow(HWindow hParent, int xPos, int yPos, unsigned int width, unsigned int height)
    {
        HWindow hWindow = m_engineContext.GetWindowManager().CreateWindow(hParent, WindowType::PopupWindow, xPos, yPos, width, height);
        if (hWindow != 0)
        {
            m_engineContext.GetWindowManager().ShowWindow(hWindow);
        }

        return hWindow;
    }

    void GameContext::DeleteWindow(HWindow hWindow)
    {
        m_engineContext.GetWindowManager().DeleteWindow(hWindow);
    }



    void GameContext::Step()
    {
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

        if ((m_flags & IsSingleThreaded) == 0)
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



    void GameContext::OnWantToClose()
    {
    }

    void GameContext::OnWindowResized(HWindow hWindow, unsigned int width, unsigned int height)
    {
    }

    void GameContext::OnWindowMoved(HWindow hWindow, int xPos, int yPos)
    {
    }

    void GameContext::OnMouseMove(HWindow hWindow, int xPos, int yPos)
    {
    }

    void GameContext::OnMouseButtonPressed(HWindow hWindow, int button, int xPos, int yPos)
    {
    }

    void GameContext::OnMouseButtonReleased(HWindow hWinodw, int button, int xPos, int yPos)
    {
    }

    void GameContext::OnMouseButtonDoubleClicked(HWindow hWindow, int button, int xPos, int yPos)
    {
    }

    void GameContext::OnKeyPressed(HWindow hWindow, GTLib::Key key)
    {
    }

    void GameContext::OnKeyReleased(HWindow hWindow, GTLib::Key key)
    {
    }

    void GameContext::OnPrintableKeyDown(HWindow hWindow, GTLib::Key key)
    {
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