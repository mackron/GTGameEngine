
#include <GTEngine/Rendering/DrawCalls/Draw.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        Draw::Draw(const VertexArray *vertexArray, DrawMode mode)
            : RenderCommand(), vertexArray(vertexArray), mode(mode)
        {
        }

        void Draw::Execute()
        {
            Renderer::Draw(this->vertexArray, this->mode);
        }

        void Draw::OnExecuted()
        {
            Draw::Deallocate(this);
        }
    }
}