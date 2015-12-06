// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Rendering/Renderer.hpp>
#include <GTGE/VertexArrayLibrary.hpp>
#include <GTGE/ShaderLibrary.hpp>
#include <GTGE/Context.hpp>

namespace GT
{
    void Renderer::Utils::DrawFullscreenQuad(Context &context, const Texture2D* texture)
    {
        if (texture != nullptr)
        {
            auto fullscreenQuadVA = context.GetVertexArrayLibrary().GetFullscreenTriangleVA();
            assert(fullscreenQuadVA != nullptr);
            {
                auto fullscreenQuadShader = context.GetShaderLibrary().GetTextured2DQuadShader();
                assert(fullscreenQuadShader != nullptr);
                {
                    // Shader setup.
                    Renderer::SetCurrentShader(fullscreenQuadShader);
                    fullscreenQuadShader->SetUniform("Projection", glm::mat4());
                    fullscreenQuadShader->SetUniform("Texture",    texture);
                    Renderer::PushPendingUniforms(*fullscreenQuadShader);

                    // Draw.
                    Renderer::DisableDepthTest();
                    Renderer::DisableDepthWrites();
                    Renderer::Draw(*fullscreenQuadVA);
                }
            }
        }
    }
}
