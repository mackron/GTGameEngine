
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTCore/Strings/Create.hpp>

namespace GTEngine
{
    Shader::Shader(const char* vertexSource, const char* fragmentSource)
        : vertexSource(GTCore::Strings::Create(vertexSource)), fragmentSource(GTCore::Strings::Create(fragmentSource)),
          pendingParameters(),
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

    void Shader::ClearPendingParameters()
    {
        this->pendingParameters.Clear();
    }
}