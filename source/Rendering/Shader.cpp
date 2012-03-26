
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderParameter.hpp>
#include <GTCore/Strings/Create.hpp>

namespace GTEngine
{
    Shader::Shader(const char* vertexSource, const char* fragmentSource)
        : vertexSource(GTCore::Strings::Create(vertexSource)), fragmentSource(GTCore::Strings::Create(fragmentSource)),
          pendingParams(),
          rendererData(nullptr)
    {
    }

    Shader::~Shader()
    {
        GTCore::Strings::Delete(this->vertexSource);
        GTCore::Strings::Delete(this->fragmentSource);

        this->ClearPendingParameters();

        Renderer::MarkForCollection(this);
    }


    void Shader::SetParameter(const char* name, float value)
    {
        this->SetParameter<ShaderParameter_Float>(name, value);
    }

    void Shader::SetParameter(const char* name, const glm::vec2 &value)
    {
        this->SetParameter<ShaderParameter_Float2>(name, value);
    }
    void Shader::SetParameter(const char* name, const glm::vec3 &value)
    {
        this->SetParameter<ShaderParameter_Float3>(name, value);
    }
    void Shader::SetParameter(const char* name, const glm::vec4 &value)
    {
        this->SetParameter<ShaderParameter_Float4>(name, value);
    }

    void Shader::SetParameter(const char* name, const glm::mat2 &value)
    {
        this->SetParameter<ShaderParameter_Float2x2>(name, value);
    }
    void Shader::SetParameter(const char* name, const glm::mat3 &value)
    {
        this->SetParameter<ShaderParameter_Float3x3>(name, value);
    }
    void Shader::SetParameter(const char* name, const glm::mat4 &value)
    {
        this->SetParameter<ShaderParameter_Float4x4>(name, value);
    }

    void Shader::SetParameter(const char* name, Texture2D* value)
    {
        this->SetParameter<ShaderParameter_Texture2D>(name, value);
    }


    void Shader::ClearPendingParameters()
    {
        for (size_t i = 0; i < this->pendingParams.count; ++i)
        {
            delete this->pendingParams.buffer[i]->value;
        }
    }
}