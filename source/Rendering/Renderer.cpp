// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>
#include <GTEngine/ShaderLibrary.hpp>

namespace GT
{
    void Renderer::Utils::DrawFullscreenQuad(const Texture2D* texture)
    {
        if (texture != nullptr)
        {
            auto fullscreenQuadVA = VertexArrayLibrary::GetFullscreenTriangleVA();
            assert(fullscreenQuadVA != nullptr);
            {
                auto fullscreenQuadShader = ShaderLibrary::GetTextured2DQuadShader();
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

#if 0
                    float triangleVertices[] =
                    {
                        -3.0f, -1.0f,
                        -1.0f,  0.0f,

                         1.0f, -1.0f,
                         1.0f,  0.0f,

                         1.0f,  3.0f,
                         1.0f,  2.0f,
                    };

                    unsigned int triangleIndices[] =
                    {
                        0, 1, 2
                    };

                    Renderer::Draw(triangleVertices, triangleIndices, 3, VertexFormat::P2T2);
#endif
                }
            }
        }
    }
}
