
#include <GTEngine/Rendering/DrawCalls/SetParameter.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTCore/Strings/Create.hpp>

namespace GTEngine
{
    namespace DrawCalls
    {
        SetParameter::SetParameter(const char *name)
            : RenderCommand(), name(GTCore::Strings::Create(name))
        {
        }

        SetParameter::~SetParameter()
        {
            GTCore::Strings::Delete(this->name);
        }
    }
}

namespace GTEngine
{
    namespace DrawCalls
    {
        // sampler2D
        SetParameter_sampler2D::SetParameter_sampler2D(const char *name, Texture2D *value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_sampler2D::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_sampler2D::OnExecuted()
        {
            SetParameter_sampler2D::Deallocate(this);
        }


        // float
        SetParameter_float::SetParameter_float(const char *name, float value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float::OnExecuted()
        {
            SetParameter_float::Deallocate(this);
        }


        // float2
        SetParameter_float2::SetParameter_float2(const char *name, const glm::vec2 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float2::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float2::OnExecuted()
        {
            SetParameter_float2::Deallocate(this);
        }

        // float3
        SetParameter_float3::SetParameter_float3(const char *name, const glm::vec3 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float3::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float3::OnExecuted()
        {
            SetParameter_float3::Deallocate(this);
        }

        // float4
        SetParameter_float4::SetParameter_float4(const char *name, const glm::vec4 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float4::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float4::OnExecuted()
        {
            SetParameter_float4::Deallocate(this);
        }


        // float2x2
        SetParameter_float2x2::SetParameter_float2x2(const char *name, const glm::mat2 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float2x2::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float2x2::OnExecuted()
        {
            SetParameter_float2x2::Deallocate(this);
        }

        // float3x3
        SetParameter_float3x3::SetParameter_float3x3(const char *name, const glm::mat3 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float3x3::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float3x3::OnExecuted()
        {
            SetParameter_float3x3::Deallocate(this);
        }

        // float4x4
        SetParameter_float4x4::SetParameter_float4x4(const char *name, const glm::mat4 &value)
            : SetParameter(name), value(value)
        {
        }

        void SetParameter_float4x4::Execute()
        {
            Renderer::SetShaderParameter(name, value);
        }

        void SetParameter_float4x4::OnExecuted()
        {
            SetParameter_float4x4::Deallocate(this);
        }
    }
}