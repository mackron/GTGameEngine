
#ifndef __GTEngine_Rendering_RCSetFramebuffer_hpp_
#define __GTEngine_Rendering_RCSetFramebuffer_hpp_

#include "../RenderCommand.hpp"
#include "../Renderer.hpp"

namespace GTEngine
{
    class Framebuffer;

    /// A super simple render command that sets the current framebuffer.
    class RCSetFramebuffer : public RenderCommand
    {
    public:

        RCSetFramebuffer()
            : framebuffer(nullptr)
        {
        }

        RCSetFramebuffer(Framebuffer* framebuffer)
            : framebuffer(framebuffer)
        {
        }

        void RCSetFramebuffer::Execute()
        {
            Renderer::SetFramebuffer(this->framebuffer);
        }


    private:

        /// The framebuffer being set. Can be null.
        Framebuffer* framebuffer;
    };
}

#endif
