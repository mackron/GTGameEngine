// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorSceneViewport.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/GameContext.hpp>

namespace GT
{
    EditorSceneViewport::EditorSceneViewport(Editor &editor)
        : EditorControl(editor),
          m_sceneCallback(), m_scene(m_sceneCallback),
          m_graphicsWorld(this->GetGUI()), m_graphicsAssetResourceManager(m_graphicsWorld, editor.GetEngineContext().GetAssetLibrary()),
          m_hViewportRTTexture(0), m_hViewportRT(0),
          m_pViewportRTTextureData(nullptr), m_viewportRTTextureDataSize(0),
          m_cursorPosXOnCapture(0), m_cursorPosYOnCapture(0), m_cursorOriginPosX(0), m_cursorOriginPosY(0), m_isLMBDown(false), m_isRMBDown(false), m_isMMBDown(false), m_isCursorCaptured(false)
    {
        if (m_graphicsWorld.Startup(editor.GetEngineContext()) && m_graphicsAssetResourceManager.Startup())
        {
            GUIContext &gui = this->GetGUI();

            HGUIElement hRootElement = this->GetRootGUIElement();
            if (hRootElement != 0)
            {
                gui.SetElementCursor(hRootElement, GUISystemCursor::Cross);


                // Painting
                gui.OnElementPaint(hRootElement, [&](GUIRenderer* pRenderer) {
                    this->Paint(pRenderer);
                });
            }
        }
    }

    EditorSceneViewport::~EditorSceneViewport()
    {
        if (m_hViewportRT != 0) {
            m_graphicsWorld.DeleteRenderTarget(m_hViewportRT);
        }

        if (m_hViewportRTTexture != 0) {
            m_graphicsWorld.DeleteResource(m_hViewportRTTexture);
        }

        m_graphicsAssetResourceManager.Shutdown();
        m_graphicsWorld.Shutdown();

        free(m_pViewportRTTextureData);
    }

    



    ///////////////////////////////////////
    // Private

    void EditorSceneViewport::OnSize(unsigned int width, unsigned int height)
    {
        if (m_hViewportRT != 0) {
            m_graphicsWorld.DeleteRenderTarget(m_hViewportRT);
            m_hViewportRT = 0;
        }

        if (m_hViewportRTTexture != 0) {
            m_graphicsWorld.DeleteResource(m_hViewportRTTexture);
            m_hViewportRTTexture = 0;
        }

        GraphicsTextureResourceDesc texDesc;
        texDesc.width  = width;
        texDesc.height = height;
        texDesc.depth  = 1;
        texDesc.format = TextureFormat_RGBA8;
        m_hViewportRTTexture = m_graphicsWorld.CreateTextureResource(texDesc);
        if (m_hViewportRTTexture != 0)
        {
            m_hViewportRT = m_graphicsWorld.CreateRenderTargetFromTexture(m_hViewportRTTexture, 0);
            m_graphicsWorld.SetRenderTargetClearColor(m_hViewportRT, GTLib::Colour(0.33f, 0.33f, 0.33f));
        }
    }

    void EditorSceneViewport::OnMouseMove(int mousePosX, int mousePosY)
    {
        // If the cursor is captured, we want to move the camera depending on which buttons are being held down.
        if (this->IsCursorCaptured())
        {
            // We want the absolute position of the cursor. We then use this to calculate the position of the cursor relative to the capture origin.
            int cursorScreenPosX;
            int cursorScreenPosY;
            if (this->RelativeToScreen(mousePosX, mousePosY, cursorScreenPosX, cursorScreenPosY))
            {
                int cursorOffsetX = cursorScreenPosX - m_cursorOriginPosX;
                int cursorOffsetY = cursorScreenPosY - m_cursorOriginPosY;
                
                if (cursorOffsetX != 0 || cursorOffsetY != 0)
                {
                    // The mouse has moved.
                    printf("%d %d\n", cursorOffsetX, cursorOffsetY);





                    // Finally, pin the cursor to the origin.
                    this->MoveCursorToCaptureOrigin();
                }
            }
        }
    }

    void EditorSceneViewport::OnMouseButtonPressed(int button, int, int)
    {
        switch (button)
        {
            case MouseButton_Left:
            {
                this->OnLMBPressed();
                break;
            }

            case MouseButton_Right:
            {
                this->OnRMBPressed();
                break;
            }

            case MouseButton_Middle:
            {
                this->OnMMBPressed();
                break;
            }

            default: break;
        }
    }

    void EditorSceneViewport::OnMouseButtonReleased(int button, int, int)
    {
        switch (button)
        {
            case MouseButton_Left:
            {
                this->OnLMBReleased();
                break;
            }

            case MouseButton_Right:
            {
                this->OnRMBReleased();
                break;
            }

            case MouseButton_Middle:
            {
                this->OnMMBReleased();
                break;
            }

            default: break;
        }
    }


    void EditorSceneViewport::Paint(GUIRenderer* pRenderer)
    {
        if (m_hViewportRT != 0 && m_hViewportRTTexture != 0)
        {
            // Draw the world first. After this we get the render target's texture data and draw it using the GUI's renderer.
            m_graphicsWorld.ExecuteRenderingCommands();


            unsigned int width;
            unsigned int height;
            unsigned int depth;  // unused.
            m_graphicsWorld.GetTextureSize(m_hViewportRTTexture, width, height, depth);

            size_t imageDataSize = width*height*4;
            if (imageDataSize > m_viewportRTTextureDataSize) {
                m_viewportRTTextureDataSize = imageDataSize;

                free(m_pViewportRTTextureData);
                m_pViewportRTTextureData = malloc(imageDataSize);
            }

            if (m_pViewportRTTextureData != nullptr)
            {
                if (m_graphicsWorld.GetTextureData(m_hViewportRTTexture, m_pViewportRTTextureData))
                {
                    int posX;
                    int posY;
                    this->GetGUI().GetElementAbsolutePosition(this->GetRootGUIElement(), posX, posY);

                    pRenderer->DrawRawImage(this->GetGUI(), posX, posY, width, height, m_pViewportRTTextureData, false);
                }
            }
        }
    }

    void EditorSceneViewport::OnLMBPressed()
    {
        if (!this->IsAnyMouseButtonDown())
        {
            this->CaptureCursor();
        }

        m_isLMBDown = true;
    }
    void EditorSceneViewport::OnRMBPressed()
    {
        if (!this->IsAnyMouseButtonDown())
        {
            this->CaptureCursor();
        }

        m_isRMBDown = true;
    }
    void EditorSceneViewport::OnMMBPressed()
    {
        if (!this->IsAnyMouseButtonDown())
        {
            this->CaptureCursor();
        }

        m_isMMBDown = true;
    }

    void EditorSceneViewport::OnLMBReleased()
    {
        m_isLMBDown = false;

        if (!this->IsAnyMouseButtonDown())
        {
            this->ReleaseCursor();
        }
    }
    void EditorSceneViewport::OnRMBReleased()
    {
        m_isRMBDown = false;

        if (!this->IsAnyMouseButtonDown())
        {
            this->ReleaseCursor();
        }
    }
    void EditorSceneViewport::OnMMBReleased()
    {
        m_isMMBDown = false;

        if (!this->IsAnyMouseButtonDown())
        {
            this->ReleaseCursor();
        }
    }

    bool EditorSceneViewport::IsAnyMouseButtonDown() const
    {
        return m_isLMBDown || m_isRMBDown || m_isMMBDown;
    }

    void EditorSceneViewport::CaptureCursor()
    {
        if (!this->IsCursorCaptured())
        {
            // We need to save the position of the cursor so it can be restored later when it is released.
            this->GetEditor().GetWindowManager().GetCursorPosition(m_cursorPosXOnCapture, m_cursorPosYOnCapture);


            // We want all mouse events to be posted to this element until the cursor is released.
            this->GetGUI().SetMouseEventCapture(this->GetRootGUIElement());

            // The cursor needs to be hidden and then moved to the center of the viewport.
            //this->GetEditor().GetWindowManager().HideCursor();


            // While captured, the cursor will pinned to a fixed position called the origin. The origin needs to be calculated now.
            this->UpdateCursorCaptureOrigin();
            this->MoveCursorToCaptureOrigin();


            // Mark the cursor as captured.
            m_isCursorCaptured = true;
        }
    }

    void EditorSceneViewport::ReleaseCursor()
    {
        if (this->IsCursorCaptured())
        {
            // While captured, the cursor will have been hidden so it needs to be shown again.
            this->GetEditor().GetWindowManager().ShowCursor();

            // We no longer want this element to be capturing mouse events either.
            this->GetGUI().ReleaseMouseEventCapture();


            // The position of the cursor needs to be moved.
            this->GetEditor().GetWindowManager().SetCursorPosition(m_cursorPosXOnCapture, m_cursorPosYOnCapture);


            // Unmark the cursor as captured.
            m_isCursorCaptured = false;
        }
    }

    bool EditorSceneViewport::IsCursorCaptured() const
    {
        return m_isCursorCaptured;
    }

    void EditorSceneViewport::UpdateCursorCaptureOrigin()
    {
        // The capture origin will be the center of the viewport.
        int viewportWidth  = int(this->GetGUI().GetElementWidth( this->GetRootGUIElement()));
        int viewportHeight = int(this->GetGUI().GetElementHeight(this->GetRootGUIElement()));

        int centerX = viewportWidth  / 2;
        int centerY = viewportHeight / 2;

        
        // At this point the center point is relative to the viewport, but we want it to be in screen coordinates. To do this, we just translate to
        // window coordinates, and then to screen coordinates.
        this->RelativeToScreen(centerX, centerY, m_cursorOriginPosX, m_cursorOriginPosY);
    }

    void EditorSceneViewport::MoveCursorToCaptureOrigin()
    {
        this->GetEditor().GetWindowManager().SetCursorPosition(m_cursorOriginPosX, m_cursorOriginPosY);
    }

    bool EditorSceneViewport::RelativeToScreen(int posXIn, int posYIn, int & posXOut, int & posYOut) const
    {
        int posXWindow;
        int posYWindow;
        this->GetGUI().RelativeToAbsolute(this->GetRootGUIElement(), posXIn, posYIn, posXWindow, posYWindow);


        // We use the window manager to convert to screen coordiantes.
        return this->GetEditor().GetWindowManager().RelativeToAbsolute(this->GetEditor().GetElementWindow(this->GetRootGUIElement()), posXWindow, posYWindow, posXOut, posYOut);
    }
}
