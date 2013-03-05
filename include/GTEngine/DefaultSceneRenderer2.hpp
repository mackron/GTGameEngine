// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer2_hpp_
#define __GTEngine_DefaultSceneRenderer2_hpp_

#include "SceneRenderer.hpp"
#include "Rendering/Renderer2.hpp"
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Structure containing information about a framebuffer that is attached to each viewport in the default scene renderer.
    ///
    /// One of these framebuffers is created for every viewport.
    struct DefaultSceneRendererFramebuffer
    {
        /// A pointer to main framebuffer object.
        Framebuffer* framebuffer;

        /// The depth/stencil buffer.
        Texture2D* depthStencilBuffer;

        /// The main colour output buffer.
        Texture2D* colourOutputBuffer;
    };


    /// Class representing the default scene renderer.
    class DefaultSceneRenderer2 : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer2();

        /// Destructor.
        virtual ~DefaultSceneRenderer2();


        ////////////////////////////////////////////////////////////////
        // Virtual Methods.

        /// SceneRenderer::Begin().
        void Begin(Scene &scene);

        /// SceneRenderer::End().
        void End(Scene &scene);

        /// SceneRenderer::RenderViewport()
        void RenderViewport(Scene &scene, SceneViewport &viewport);

        /// SceneRenderer::AddViewport()
        void AddViewport(SceneViewport &viewport);

        /// SceneRenderer::RemoveViewport()
        void RemoveViewport(SceneViewport &viewport);

        /// SceneRenderer::OnViewportResized()
        void OnViewportResized(SceneViewport &viewport);


    private:

        /// Creates a new framebuffer of the given dimensions.
        ///
        /// @param width  [in] The width of the new framebuffer.
        /// @param height [in] The height of the new framebuffer.
        DefaultSceneRendererFramebuffer* CreateFramebuffer(unsigned int width, unsigned int height);

        /// Deletes the given framebuffer that was created with CreateFramebuffer().
        ///
        /// @param framebufferToDelete [in] A pointer to the framebuffer being deleted. This must have been created with CreateFramebuffer().
        void DeleteFramebuffer(DefaultSceneRendererFramebuffer* framebufferToDelete);

        /// Retrieves a pointer to the framebuffer of the given viewport.
        ///
        /// @param viewport [in] A reference to the viewport whose framebuffer is being retrieved.
        DefaultSceneRendererFramebuffer* GetViewportFramebuffer(SceneViewport &viewport);



    private:

        /// The framebuffers for each attached viewport. Keyed by the viewport.
        GTCore::Map<SceneViewport*, DefaultSceneRendererFramebuffer*> viewportFramebuffers;


    };
}

#endif