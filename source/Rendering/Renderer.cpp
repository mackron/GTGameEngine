// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/ShaderLibrary.hpp>

namespace GTEngine
{
    void Renderer::Utils::DrawFullscreenQuad(const Texture2D* texture)
    {
        if (texture != nullptr)
        {
            auto fullscreenQuadVA = GTEngine::VertexArrayLibrary::GetFullscreenTriangleVA();
            assert(fullscreenQuadVA != nullptr);
            {
                auto fullscreenQuadShader = GTEngine::ShaderLibrary::GetTextured2DQuadShader();
                assert(fullscreenQuadShader != nullptr);
                {
                    // Shader setup.
                    GTEngine::Renderer::SetCurrentShader(fullscreenQuadShader);
                    fullscreenQuadShader->SetUniform("Projection", glm::mat4());
                    fullscreenQuadShader->SetUniform("Texture",    texture);
                    GTEngine::Renderer::PushPendingUniforms(*fullscreenQuadShader);

                    // Draw.
                    GTEngine::Renderer::DisableDepthTest();
                    GTEngine::Renderer::DisableDepthWrites();
                    GTEngine::Renderer::Draw(*fullscreenQuadVA);
                }
            }
        }
    }
}
