// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorSceneViewport
#define GT_EditorSceneViewport

#include "EditorControl.hpp"
#include "../../Scene/DefaultSceneCallback.hpp"
#include "../../Scene/Scene.hpp"
#include "../../Graphics/DefaultGraphicsWorld.hpp"
#include "../../Graphics/GraphicsAssetResourceManager.hpp"

namespace GT
{
    class EditorSceneViewport : public EditorControl
    {
    public:

        /// Constructor
        EditorSceneViewport(Editor &editor);

        /// Destructor.
        ~EditorSceneViewport();


        /// Retrieves a reference to the scene.
        Scene & GetScene() { return m_scene; }

        /// Retrieves a reference to the graphics world.
        GraphicsWorld & GetGraphicsWorld() { return m_graphicsWorld; }



    private:

        /// @copydoc EditorControl::OnSize()
        void OnSize(unsigned int width, unsigned int height);

        /// @copydoc EditorControl::OnMouseMove()
        void OnMouseMove(int mousePosX, int mousePosY);

        /// @copydoc EditorControl::OnMouseButtonPressed()
        void OnMouseButtonPressed(int button, int posX, int posY);

        /// @copydoc EditorControl::OnMouseButtonReleased()
        void OnMouseButtonReleased(int button, int posX, int posY);

        
        /// Paints the viewport.
        void Paint(GUIRenderer* pRenderer);



        void OnLMBPressed();
        void OnRMBPressed();
        void OnMMBPressed();

        void OnLMBReleased();
        void OnRMBReleased();
        void OnMMBReleased();

        bool IsAnyMouseButtonDown() const;

        void CaptureCursor();
        void ReleaseCursor();
        bool IsCursorCaptured() const;

        void UpdateCursorCaptureOrigin();
        void MoveCursorToCaptureOrigin();

        bool RelativeToScreen(int posXIn, int posYIn, int &posXOut, int &posYOut) const;


    private:

        /// The scene's callback.
        DefaultSceneCallback m_sceneCallback;

        /// The scene.
        Scene m_scene;


        /// The graphics world that will draw the scene.
        DefaultGraphicsWorld m_graphicsWorld;

        /// The graphics asset resource manager. This is needed in order to load resources like meshes and whatnot.
        GraphicsAssetResourceManager m_graphicsAssetResourceManager;


        /// The render target target texture for the viewport.
        HGraphicsResource m_hViewportRTTexture;

        /// The texture render target for the viewport.
        HGraphicsRenderTarget m_hViewportRT;

        
        /// The buffer used to store the raw image data of the render target's texture. We need this so we can pass it to the GUI's renderer
        /// and draw the render target to the window. Later on we'll look at how to render this directly to the window using a window render
        /// target.
        ///
        /// This buffer will grow as required, but never shrink.
        void* m_pViewportRTTextureData;

        /// The size of the buffer at m_pViewportRTTextureData.
        size_t m_viewportRTTextureDataSize;


        /// The position of the mouse at the time it was hidden. This is used to reposition the mouse upon release.
        int m_cursorPosXOnCapture;
        int m_cursorPosYOnCapture;

        /// The position of the mouse cursor at it's origin while captured.
        int m_cursorOriginPosX;
        int m_cursorOriginPosY;

        /// Keeps track of whether or not a mouse button is down.
        bool m_isLMBDown;
        bool m_isRMBDown;
        bool m_isMMBDown;

        /// Keeps track of whether or not the cursor is captured.
        bool m_isCursorCaptured;
    };
}

#endif
