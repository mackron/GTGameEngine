
#include <GTEngine/Rendering/RenderCommands/RCSetFramebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    RCSetFramebuffer::RCSetFramebuffer()
        : framebuffer(nullptr)
    {
    }
    
    RCSetFramebuffer::RCSetFramebuffer(Framebuffer* framebuffer)
        : framebuffer(framebuffer)
    {
    }

    void RCSetFramebuffer::Execute()
    {
        Renderer::SetFramebuffer(this->framebuffer);
    }
}
