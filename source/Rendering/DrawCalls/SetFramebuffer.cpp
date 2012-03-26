
#include <GTEngine/Rendering/DrawCalls/SetFramebuffer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        SetFramebuffer::SetFramebuffer(Framebuffer *framebuffer)
            : RenderCommand(), framebuffer(framebuffer)
        {
        }

        void SetFramebuffer::Execute()
        {
            Renderer::SetFramebuffer(this->framebuffer);
        }

        void SetFramebuffer::OnExecuted()
        {
            SetFramebuffer::Deallocate(this);
        }
    }
}