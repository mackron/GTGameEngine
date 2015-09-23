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

        /// Resizes the viewport.
        void Resize(unsigned int width, unsigned int height);

        /// Paints the viewport.
        void Paint(GUIRenderer* pRenderer);


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
    };
}

#endif
