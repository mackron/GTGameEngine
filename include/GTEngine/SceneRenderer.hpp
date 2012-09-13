
#ifndef __GTEngine_SceneRenderer_hpp_
#define __GTEngine_SceneRenderer_hpp_

#include "SceneViewport.hpp"

namespace GTEngine
{
    /// Class responsible for the rendering a scene.
    class SceneRenderer
    {
    public:

        /// Constructor.
        SceneRenderer() {}

        /// Destructor.
        virtual ~SceneRenderer() {}


        /// Called when rendering has started for the next frame.
        ///
        /// @remarks
        ///     This will be called once for every frame.
        virtual void Begin(Scene &scene) = 0;

        /// Called when rendering has ended.
        ///
        /// @remarks
        ///     This will be called once for every frame, after rendering has finished.
        virtual void End(Scene &scene) = 0;


        /// Called when rendering has started for the given viewport.
        ///
        /// @remarks viewport [in] The viewport that has started rendering.
        ///
        /// @remarks
        ///     This will be called once for every viewport.
        virtual void RenderViewport(Scene &scene, SceneViewport &viewport) = 0;


        /// Called when a viewport needs to be added to the renderer.
        ///
        /// @param viewport [in] A reference to the viewport this renderer will need to manage.
        virtual void AddViewport(SceneViewport &viewport) = 0;

        /// Called when a viewport needs to be removed from the renderer.
        ///
        /// @param viewport [in] A reference to the viewport this renderer will stop managing.
        virtual void RemoveViewport(SceneViewport &viewport) = 0;

        /// Called after a viewport has been resized.
        ///
        /// @param viewport [in] The viewport being resized.
        ///
        /// @remarks
        ///     This function is needed so that the renderer can resize the internal framebuffers and whatnot.
        virtual void OnViewportResized(SceneViewport &viewport) = 0;


        /// Enables background colour clearing.
        ///
        /// @param r [in] The red component of the clear colour.
        /// @param g [in] The green component of the clear colour.
        /// @param b [in] The blue component of the clear colour.
        virtual void EnableBackgroundColourClearing(float r, float g, float b) = 0;

        /// Disables background colour clearing.
        virtual void DisableBackgroundColourClearing() = 0;
    };
}

#endif
