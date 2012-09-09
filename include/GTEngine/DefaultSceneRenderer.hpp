
#ifndef __GTEngine_DefaultSceneRenderer_hpp_
#define __GTEngine_DefaultSceneRenderer_hpp_

#include "SceneRenderer.hpp"
#include "SceneObject.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/RenderCommand.hpp"
#include "Rendering/RCCache.hpp"
#include <GTCore/Map.hpp>

class Scene;

namespace GTEngine
{
    /// The default scene renderer.
    class DefaultSceneRenderer : public SceneRenderer
    {
    public:

        /// Constructor.
        DefaultSceneRenderer();

        /// Destructor.
        virtual ~DefaultSceneRenderer();


        /// SceneRenderer::Begin().
        virtual void Begin(Scene &scene);

        /// SceneRenderer::End().
        virtual void End(Scene &scene);


        /// SceneRenderer::BeginViewport().
        virtual void RenderViewport(Scene &scene, SceneViewport &viewport);


        /// SceneRenderer::AddViewport().
        virtual void AddViewport(SceneViewport &viewport);

        /// SceneRenderer::RemoveViewport().
        virtual void RemoveViewport(SceneViewport &viewport);

        /// SceneRenderer::OnViewportResized().
        virtual void OnViewportResized(SceneViewport &viewport);


        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // Methods below should only be called internally, but need to be public for a few things.

        /// Called for an ambient light that's visible in the currently rendering viewport.
        void __AmbientLight(const SceneObject &object);

        /// Called for a directional light that's visible in the currently rendering viewport.
        void __DirectionalLight(const SceneObject &object);

        /// Called for a point light that's visible in the currently rendering viewport.
        void __PointLight(const SceneObject &object);

        /// Called for a spot light that's visible in the currently rendering viewport.
        void __SpotLight(const SceneObject &object);



    // Below are data structures for use by this renderer.
    private:

        /// Class representing a framebuffer for use by this renderer. There will usually be a framebuffer for every viewport.
        class Framebuffer : public GTEngine::Framebuffer
        {
        public:

            /// Constructor.
            Framebuffer();

            /// Destructor
            ~Framebuffer();


            /// Marks the framebuffer for resizing.
            void Resize(unsigned int newWidth, unsigned int newHeight)
            {
                this->width  = newWidth;
                this->height = newHeight;

                this->needsResize = true;
            }


        private:

            /// The width of the framebuffer.
            unsigned int width;

            /// The height of the framebuffer.
            unsigned int height;

            /// Keeps track of whether or not the framebuffer needs a resize.
            bool needsResize;
        };



        /// Render command for beginning the frame.
        struct RCBegin : public RenderCommand
        {
            /// RenderCommand::Execute().
            void Execute();


            /// The framebuffer to render to.
            Framebuffer* framebuffer;
        };

        /// Render command for ending the frame.
        struct RCEnd : public RenderCommand
        {
            /// RenderCommand::Execute().
            void Execute();


            /// Whether or not the draw the background.
            bool drawBackground;

            /// The background colour.
            glm::vec3 backgroundColour;
        };



    private:

        /// Retrieves the framebuffer of the given viewport.
        DefaultSceneRenderer::Framebuffer* GetViewportFramebuffer(SceneViewport &viewport);




    private:

        /// The list of visible ambient lights for the currently rendering viewport.
        GTCore::Vector<const SceneObject*> ambientLights;

        /// The list of visible directional lights for the currently rendering viewport.
        GTCore::Vector<const SceneObject*> directionalLights;

        /// The list of visible point lights for the currently rendering viewport.
        GTCore::Vector<const SceneObject*> pointLights;

        /// The list of visible spot lights for the currently rendering viewport.
        GTCore::Vector<const SceneObject*> spotLights;


        /// A container for mapping a viewport to it's framebuffer.
        GTCore::Map<SceneViewport*, DefaultSceneRenderer::Framebuffer*> viewportFramebuffers;


        // Below are caches for render commands. There are always 2 caches - one for the front RC queue, and another for the back.
        RCCache<RCBegin, 8> rcBegin[2];


        
    };
}

#endif