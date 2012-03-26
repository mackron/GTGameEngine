
#include <GTEngine/Rendering/DrawCalls/SetScissor.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        SetScissor::SetScissor(int x, int y, unsigned int width, unsigned int height)
            : RenderCommand(), x(x), y(y), width(width), height(height)
        {
        }

        void SetScissor::Execute()
        {
            Renderer::SetViewport(this->x, this->y, this->width, this->height);
        }

        void SetScissor::OnExecuted()
        {
            SetScissor::Deallocate(this);
        }
    }
}