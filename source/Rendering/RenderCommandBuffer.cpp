
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
    
    void RenderCommandBuffer::Append(RenderCommand &cmd)
    {
        this->commands.PushBack(&cmd);
    }

    void RenderCommandBuffer::Execute()
    {
        for (size_t i = 0; i < this->commands.count; ++i)
        {
            auto command = this->commands.buffer[i];
            command->Execute();

            // TODO: This event needs to be removed. It exists now because I needed a way to know when to delete the objects. Instead, 
            // commands should be cached and managed manually.
            command->OnExecuted();
        }
    }

    void RenderCommandBuffer::Clear()
    {
        // Just to ensure we don't cycle over and attempt to call destructors, we'll simply set the count to 0
        // which is enough to clear the buffer. We would otherwise use this->commands.Clear();
        this->commands.count = 0;
    }
}

// Draw call helpers.
namespace GTEngine
{
    void RenderCommandBuffer::SetViewport(int x, int y, unsigned int width, unsigned int height)
    {
        this->Append(*DrawCalls::SetViewport::Allocate(x, y, width, height));
    }

    void RenderCommandBuffer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        this->Append(*DrawCalls::SetScissor::Allocate(x, y, width, height));
    }

    void RenderCommandBuffer::Draw(const VertexArray *vertexArray, DrawMode mode)
    {
        this->Append(*DrawCalls::Draw::Allocate(vertexArray, mode));
    }

    void RenderCommandBuffer::SetShader(Shader *shader)
    {
        this->Append(*DrawCalls::SetShader::Allocate(shader));
    }


    void RenderCommandBuffer::SetShaderParameter(const char *name, Texture2D *value)
    {
        this->Append(*DrawCalls::SetParameter_sampler2D::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, float value)
    {
        this->Append(*DrawCalls::SetParameter_float::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec2 &value)
    {
        this->Append(*DrawCalls::SetParameter_float2::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec3 &value)
    {
        this->Append(*DrawCalls::SetParameter_float3::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::vec4 &value)
    {
        this->Append(*DrawCalls::SetParameter_float4::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat2 &value)
    {
        this->Append(*DrawCalls::SetParameter_float2x2::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat3 &value)
    {
        this->Append(*DrawCalls::SetParameter_float3x3::Allocate(name, value));
    }
    void RenderCommandBuffer::SetShaderParameter(const char *name, const glm::mat4 &value)
    {
        this->Append(*DrawCalls::SetParameter_float4x4::Allocate(name, value));
    }


    void RenderCommandBuffer::SetFramebuffer(Framebuffer *framebuffer)
    {
        this->Append(*DrawCalls::SetFramebuffer::Allocate(framebuffer));
    }
}
