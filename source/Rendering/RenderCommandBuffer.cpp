
#include <GTEngine/Rendering/RenderCommandBuffer.hpp>
#include <GTEngine/Rendering/DrawCalls/SetViewport.hpp>
#include <GTEngine/Rendering/DrawCalls/SetScissor.hpp>
#include <GTEngine/Rendering/DrawCalls/Draw.hpp>
#include <GTEngine/Rendering/DrawCalls/SetShader.hpp>
#include <GTEngine/Rendering/DrawCalls/SetParameter.hpp>
#include <GTEngine/Rendering/DrawCalls/SetFramebuffer.hpp>
#include <GTEngine/Material.hpp>
#include <cassert>

namespace GTEngine
{
    RenderCommandBuffer::RenderCommandBuffer()
        : commands()
    {
    }
    
    void RenderCommandBuffer::Append(RenderCommand *cmd)
    {
        assert(cmd != nullptr);

        this->commands.Append(cmd);
    }

    void RenderCommandBuffer::Execute()
    {
        for (auto i = this->commands.root; i != nullptr; i = i->next)
        {
            i->value->Execute();
            i->value->OnExecuted();
        }
    }

    void RenderCommandBuffer::Clear()
    {
        this->commands.Clear();
    }
}

// Draw call helpers.
namespace GTEngine
{
    void RenderCommandBuffer::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
        this->Append(DrawCalls::SetViewport::Allocate(x, y, width, height));
    }

    void RenderCommandBuffer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        this->Append(DrawCalls::SetScissor::Allocate(x, y, width, height));
    }

    void RenderCommandBuffer::Draw(const VertexArray *vertexArray, DrawMode mode)
    {
        this->Append(DrawCalls::Draw::Allocate(vertexArray, mode));
    }

    void RenderCommandBuffer::SetShader(Shader *shader)
    {
        this->Append(DrawCalls::SetShader::Allocate(shader));
    }


    void RenderCommandBuffer::SetShaderParameter(const char *name, Texture2D *value)
    {
        this->Append(DrawCalls::SetParameter_sampler2D::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, float value)
    {
        this->Append(DrawCalls::SetParameter_float::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec2 &value)
    {
        this->Append(DrawCalls::SetParameter_float2::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec3 &value)
    {
        this->Append(DrawCalls::SetParameter_float3::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec4 &value)
    {
        this->Append(DrawCalls::SetParameter_float4::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat2 &value)
    {
        this->Append(DrawCalls::SetParameter_float2x2::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat3 &value)
    {
        this->Append(DrawCalls::SetParameter_float3x3::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat4 &value)
    {
        this->Append(DrawCalls::SetParameter_float4x4::Allocate(name, value));
    }


    void RenderCommandBuffer::SetFramebuffer(Framebuffer *framebuffer)
    {
        this->Append(DrawCalls::SetFramebuffer::Allocate(framebuffer));
    }
}
