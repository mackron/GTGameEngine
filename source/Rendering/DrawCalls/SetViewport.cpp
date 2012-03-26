
#include <GTEngine/Rendering/DrawCalls/SetViewport.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        SetViewport::SetViewport(int x, int y, unsigned int width, unsigned int height)
            : RenderCommand(), x(x), y(y), width(width), height(height)
        {
        }

        void SetViewport::Execute()
        {
            Renderer::SetViewport(this->x, this->y, this->width, this->height);
        }

        void SetViewport::OnExecuted()
        {
            SetViewport::Deallocate(this);
        }
    }
}