// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Material.hpp>

namespace GTEngine
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

    void Shader::SetUniformsFromMaterial(Material &material)
    {
        auto &materialUniforms = material.GetParameters();

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
            }
        }
    }
}
