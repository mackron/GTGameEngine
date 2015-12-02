// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Rendering/Shader.hpp>
#include <GTGE/Material.hpp>

namespace GT
{
    Shader::Shader(const char* vertexSourceIn, const char* fragmentSourceIn, const char* geometrySourceIn)
        : vertexSource(vertexSourceIn), fragmentSource(fragmentSourceIn), geometrySource(geometrySourceIn)
    {
    }

    Shader::~Shader()
    {
    }


    //////////////////////////////////////////////
    // Uniforms.

    void Shader::SetUniform(const char*, float)
    {
    }

    void Shader::SetUniform(const char*, float, float)
    {
    }

    void Shader::SetUniform(const char*, float, float, float)
    {
    }

    void Shader::SetUniform(const char*, float, float, float, float)
    {
    }

    void Shader::SetUniform(const char*, const glm::mat2 &)
    {
    }

    void Shader::SetUniform(const char*, const glm::mat3 &)
    {
    }

    void Shader::SetUniform(const char*, const glm::mat4 &)
    {
    }

    void Shader::SetUniform(const char*, const Texture2D*)
    {
    }

    void Shader::SetUniform(const char*, const TextureCube*)
    {
    }

    void Shader::SetUniformsFromMaterial(const Material &material)
    {
        auto &materialUniforms = material.GetParameters();

        auto &floatUniforms       = materialUniforms.GetFloatParameters();
        auto &float2Uniforms      = materialUniforms.GetFloat2Parameters();
        auto &float3Uniforms      = materialUniforms.GetFloat3Parameters();
        auto &float4Uniforms      = materialUniforms.GetFloat4Parameters();
        auto &float2x2Uniforms    = materialUniforms.GetFloat2x2Parameters();
        auto &float3x3Uniforms    = materialUniforms.GetFloat3x3Parameters();
        auto &float4x4Uniforms    = materialUniforms.GetFloat4x4Parameters();
        auto &texture2DUniforms   = materialUniforms.GetTexture2DParameters();
        auto &textureCubeUniforms = materialUniforms.GetTextureCubeParameters();

        
        // Float
        for (size_t i = 0; i < floatUniforms.count; ++i)
        {
            auto name  = floatUniforms.buffer[i]->key;
            auto value = floatUniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float2
        for (size_t i = 0; i < float2Uniforms.count; ++i)
        {
            auto name  = float2Uniforms.buffer[i]->key;
            auto value = float2Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float3
        for (size_t i = 0; i < float3Uniforms.count; ++i)
        {
            auto name  = float3Uniforms.buffer[i]->key;
            auto value = float3Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float4
        for (size_t i = 0; i < float4Uniforms.count; ++i)
        {
            auto name  = float4Uniforms.buffer[i]->key;
            auto value = float4Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float2x2
        for (size_t i = 0; i < float2x2Uniforms.count; ++i)
        {
            auto name  = float2x2Uniforms.buffer[i]->key;
            auto value = float2x2Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float3x3
        for (size_t i = 0; i < float3x3Uniforms.count; ++i)
        {
            auto name  = float3x3Uniforms.buffer[i]->key;
            auto value = float3x3Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }

        // Float4x4
        for (size_t i = 0; i < float4x4Uniforms.count; ++i)
        {
            auto name  = float4x4Uniforms.buffer[i]->key;
            auto value = float4x4Uniforms.buffer[i]->value.value;
            
            this->SetUniform(name, value);
        }


        // Texture2D
        for (size_t i = 0; i < texture2DUniforms.count; ++i)
        {
            auto name  = texture2DUniforms.buffer[i]->key;
            auto value = texture2DUniforms.buffer[i]->value.value;
            
            if (value != nullptr)
            {
                this->SetUniform(name, value);
            }
        }

        // TextureCube
        for (size_t i = 0; i < textureCubeUniforms.count; ++i)
        {
            auto name  = textureCubeUniforms.buffer[i]->key;
            auto value = textureCubeUniforms.buffer[i]->value.value;
            
            if (value != nullptr)
            {
                this->SetUniform(name, value);
            }
        }


#if 0
        for (size_t i = 0; i < materialUniforms.count; ++i)
        {
            auto uniformName  = materialUniforms.buffer[i]->key;
            auto uniformValue = materialUniforms.buffer[i]->value;

            switch (uniformValue->type)
            {
            case ShaderParameterType_Float:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_Float2:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float2*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_Float3:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float3*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_Float4:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float4*>(uniformValue)->value);
                    break;
                }


            case ShaderParameterType_Float2x2:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float2x2*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_Float3x3:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float3x3*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_Float4x4:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Float4x4*>(uniformValue)->value);
                    break;
                }


            case ShaderParameterType_Texture2D:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_Texture2D*>(uniformValue)->value);
                    break;
                }

            case ShaderParameterType_TextureCube:
                {
                    this->SetUniform(uniformName, static_cast<ShaderParameter_TextureCube*>(uniformValue)->value);
                    break;
                }
                
            
            case ShaderParameterType_Integer:
            case ShaderParameterType_Integer2:
            case ShaderParameterType_Integer3:
            case ShaderParameterType_Integer4:
            case ShaderParameterType_Texture1D:
            case ShaderParameterType_Texture3D:
            default: break;
            }
        }
#endif
    }
}
