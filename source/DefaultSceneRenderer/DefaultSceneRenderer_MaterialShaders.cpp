// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_MaterialShaders.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_MaterialShaders::DefaultSceneRenderer_MaterialShaders()
        : ambientLightShader(nullptr),
          directionalLightShader(nullptr),
          pointLightShader(nullptr),
          spotLightShader(nullptr),
          shadowDirectionalLightShader(nullptr),
          shadowPointLightShader(nullptr),
          shadowSpotLightShader(nullptr),
          materialShader(nullptr)
    {
    }

    DefaultSceneRenderer_MaterialShaders::~DefaultSceneRenderer_MaterialShaders()
    {
        Renderer::DeleteShader(this->ambientLightShader);
        Renderer::DeleteShader(this->directionalLightShader);
        Renderer::DeleteShader(this->pointLightShader);
        Renderer::DeleteShader(this->spotLightShader);
        Renderer::DeleteShader(this->shadowDirectionalLightShader);
        Renderer::DeleteShader(this->shadowPointLightShader);
        Renderer::DeleteShader(this->shadowSpotLightShader);
        Renderer::DeleteShader(this->materialShader);
    }
}
