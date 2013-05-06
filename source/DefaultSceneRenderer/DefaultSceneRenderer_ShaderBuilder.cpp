// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_ShaderBuilder.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_ShaderBuilder::DefaultSceneRenderer_ShaderBuilder()
        : cachedShaders()
    {
    }

    DefaultSceneRenderer_ShaderBuilder::~DefaultSceneRenderer_ShaderBuilder()
    {
        for (size_t i = 0; i < this->cachedShaders.count; ++i)
        {
            auto shader = this->cachedShaders.buffer[i]->value;
            assert(shader != nullptr);
            {
                Renderer::DeleteShader(shader);
            }
        }
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const Material* material)
    {
        ShaderKey shaderKey(shaderID, material);

        auto iShader = this->cachedShaders.Find(shaderKey);
        if (iShader != nullptr)
        {
            return iShader->value;
        }


        // If we made it here it means that we need to create a new shader.
        return nullptr;
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const Material &material)
    {
        return this->CreateShader(shaderID, &material);
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID)
    {
        return this->CreateShader(shaderID, nullptr);
    }
}