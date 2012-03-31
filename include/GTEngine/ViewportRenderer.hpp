
#ifndef __GTEngine_ViewportRenderer_hpp_
#define __GTEngine_ViewportRenderer_hpp_

#include <GTCore/Vector.hpp>

namespace GTEngine
{
    class SceneNode;
    class SceneViewport;
    class Framebuffer;
    class Texture2D;

    /// Base class for rendering the contents of a scene's viewport.
    ///
    /// Viewport renderers always render to a framebuffer object, and never the main framebuffer. We do this for ease-of-use and flexibility.
    class ViewportRenderer
    {
    public:

        /// Constructor.
        ViewportRenderer();

        /// Destructor.
        virtual ~ViewportRenderer();


        /// Sets the viewport this renderer is attached to. This should only be called by SceneViewport::SetRenderer().
        virtual void SetOwnerViewport(SceneViewport* owner) = 0;

        /// Retrieves a pointer to the viewport that this renderer is attached to.
        virtual SceneViewport* GetOwnerViewport() = 0;


        /// Retrieves a pointer to the framebuffer rendering has occured on.
        virtual Framebuffer & GetFramebuffer() = 0;

        /// Called by the owning viewport when it is resized.
        /// @param newWidth  [in] The new width of the framebuffer.
        /// @param newHeight [in] The new height of the framebuffer.
        ///
        /// @remarks
        ///     This will not be called on the rendering thread. It's recommended to cache the new framebuffer size and do the
        ///     actual resize in OnSwapRCQueues().
        virtual void ResizeFramebuffer(unsigned int newWidth, unsigned int newHeight) = 0;


        /// Renders the contents of the viewport this renderer is attached to.
        ///
        /// @remarks
        ///     The final output will be stored on a framebuffer than can be retrieved with GetFramebuffer().
        ///     @par
        ///     Rendering is done from the perspective of the camera on the owner viewport.
        ///     @par
        ///     This will be called on the update thread. Implementations must use render commands in this function instead of
        ///     direct rendering operations.
        virtual void Render() = 0;

        /// Called when the main renderer's RC buffers have been swapped. This occurs once every frame. Renderer's need to know
        /// about this because they may need to manage their rendering command objects.
        ///
        /// This function is called synchronously between frames, so it's a good time to do any rendering object modifications if
        /// required. In particular, doing the actual framebuffer resize might be best done here.
        virtual void OnSwapRCQueues() = 0;


        /// Retrieves the buffer containing the final colour output.
        virtual Texture2D* GetFinalColourOutputBuffer() = 0;

        /// Retrieves the buffer containing the final depth/stencil output.
        virtual Texture2D* GetFinalDepthStencilOutputBuffer() = 0;


    protected:


    private:    // No copying.
        ViewportRenderer(const ViewportRenderer &);
        ViewportRenderer & operator=(const ViewportRenderer &);
    };
}


#endif