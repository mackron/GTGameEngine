// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorSceneViewport.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorSceneViewport::EditorSceneViewport(Editor &editor)
        : EditorControl(editor),
          m_sceneCallback(), m_scene(m_sceneCallback),
          m_graphicsWorld(this->GetGUI()), m_graphicsAssetResourceManager(m_graphicsWorld, editor.GetEngineContext().GetAssetLibrary()),
          m_hViewportRTTexture(0), m_hViewportRT(0)
    {
        if (m_graphicsWorld.Startup(editor.GetEngineContext()) && m_graphicsAssetResourceManager.Startup())
        {
            GUIContext &gui = this->GetGUI();

            HGUIElement hRootElement = this->GetRootGUIElement();
            if (hRootElement != 0)
            {
                // When the element changes size we need to recreate the render target.
                gui.OnElementSize(hRootElement, [&](unsigned int width, unsigned int height) {
                    this->Resize(width, height);
                });

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
    }

    



    ///////////////////////////////////////
    // Private

    void EditorSceneViewport::Resize(unsigned int width, unsigned int height)
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

            void* pImageData = malloc(width * height * 4);      // Should probably allocate this once and re-alloc on resize.
            if (pImageData != nullptr)
            {
                if (m_graphicsWorld.GetTextureData(m_hViewportRTTexture, pImageData))
                {
                    int posX;
                    int posY;
                    this->GetGUI().GetElementAbsolutePosition(this->GetRootGUIElement(), posX, posY);

                    pRenderer->DrawRawImage(this->GetGUI(), posX, posY, width, height, pImageData, false);
                }
                            
                free(pImageData);
            }
        }
    }
}
