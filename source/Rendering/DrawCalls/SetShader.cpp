
#include <GTEngine/Rendering/DrawCalls/SetShader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        SetShader::SetShader(Shader *shader)
            : RenderCommand(), shader(shader)
        {
        }

        void SetShader::Execute()
        {
            Renderer::SetShader(this->shader);
        }

        void SetShader::OnExecuted()
        {
            SetShader::Deallocate(this);
        }
    }
}