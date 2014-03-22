// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_LuminanceChain.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/VertexArrayLibrary.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_LuminanceChain::DefaultSceneRenderer_LuminanceChain()
        : m_framebuffer(),
          m_buffers(),
          m_shaders(),
          m_baseWidth(0),
          m_baseHeight(0)
    {
        this->m_framebuffer = Renderer::CreateFramebuffer();
    }

    DefaultSceneRenderer_LuminanceChain::~DefaultSceneRenderer_LuminanceChain()
    {
        Renderer::DeleteFramebuffer(this->m_framebuffer);

        this->DeleteAllShaders();
        this->DeleteAllBuffers();
    }


    void DefaultSceneRenderer_LuminanceChain::ComputeLuminance(const Texture2D &sourceImage, float exposure)
    {
        // Resize and construct the chain.
        if (this->Initialize(sourceImage.GetWidth(), sourceImage.GetHeight()))
        {
            // Set the framebuffer.
            Renderer::SetCurrentFramebuffer(this->m_framebuffer);

            int attachmentIndex = 0;
            Renderer::SetDrawBuffers(1, &attachmentIndex);
            

            // Set misc. state.
            Renderer::DisableDepthWrites();
            Renderer::DisableDepthTest();
            Renderer::DisableBlending();

            for (size_t iBuffer = 0; iBuffer < this->m_buffers.count; ++iBuffer)
            {
                // We now need to check if any existing attachments are different to those we want to be attached. If so, we need
                // to change them.
                bool framebufferNeedsPush = false;
                auto &currentAttachments = this->m_framebuffer->GetAttachedColourBuffers();

                
                Texture2D* outputTexture = this->m_buffers[iBuffer];
                assert(outputTexture != nullptr);
                {
                    if (currentAttachments.count == 0 || currentAttachments.buffer[0]->value != outputTexture)
                    {
                        this->m_framebuffer->DetachColourBuffer(0);
                        this->m_framebuffer->AttachColourBuffer(outputTexture, 0);
                        framebufferNeedsPush = true;
                    }
                }

                if (framebufferNeedsPush)
                {
                    Renderer::PushAttachments(*this->m_framebuffer);
                }


                // The framebuffer is setup so now we need to actually render each downsampled buffer.
                const Texture2D* inputTexture;
                if (iBuffer == 0)
                {
                    // Special case. The input texture needs to be the source image.
                    inputTexture = &sourceImage;
                }
                else
                {
                    // Normal case. The input texture is the previous texture in the chain.
                    inputTexture = this->m_buffers[iBuffer - 1];
                }


                assert(inputTexture  != nullptr);
                {
                    // Set the viewport.
                    Renderer::SetViewport(0, 0, outputTexture->GetWidth(), outputTexture->GetHeight());
                    
                    // Set the shader.
                    auto shader = this->m_shaders[iBuffer];
                    assert(shader != nullptr);
                    {
                        Renderer::SetCurrentShader(shader);
                        shader->SetUniform("InputTexture", inputTexture);

                        if (iBuffer == this->m_buffers.count - 1)
                        {
                            shader->SetUniform("Exposure", glm::vec2(exposure, exposure * (1.0f / (exposure + 1.0f))));
                        }

                        Renderer::PushPendingUniforms(shader);
                    }

                    // Set the quad.
                    Renderer::Draw(VertexArrayLibrary::GetFullscreenTriangleVA());
                }
            }
        }
    }

    const Texture2D* DefaultSceneRenderer_LuminanceChain::GetLuminanceBuffer() const
    {
        if (this->m_buffers.count > 0)
        {
            return this->m_buffers.GetBack();
        }

        return nullptr;
    }


    /////////////////////////////////////////////////////
    // Private

    bool DefaultSceneRenderer_LuminanceChain::Initialize(unsigned int baseWidth, unsigned int baseHeight)
    {
        // Check if we actually need to resize. If we don't, we don't bother doing anything.
        if (this->m_baseWidth != baseWidth || this->m_baseHeight != baseHeight)
        {
            this->m_baseWidth  = baseWidth;
            this->m_baseHeight = baseHeight;

            // All existing shaders need to be deleted because they will be re-created.
            this->DeleteAllShaders();

            // We need to resize the buffers.
            unsigned int chainCount = 0;
            
            unsigned int prevWidth  = baseWidth;
            unsigned int prevHeight = baseHeight;
            unsigned int nextWidth  = baseWidth;
            unsigned int nextHeight = baseHeight;
            while (nextWidth > 1 && nextHeight > 1)
            {
                nextWidth  = GTLib::Max(1U, nextWidth  / 4);
                nextHeight = GTLib::Max(1U, nextHeight / 4);

                // If it's the first link in the chain, we will raise to the closes power of 4.
                if (chainCount == 0)
                {
                    nextWidth  = static_cast<unsigned int>(glm::pow(4.0f, glm::ceil(glm::log(static_cast<float>(nextWidth))  / glm::log(4.0f))));
                    nextHeight = static_cast<unsigned int>(glm::pow(4.0f, glm::ceil(glm::log(static_cast<float>(nextHeight)) / glm::log(4.0f))));

                    nextWidth = nextHeight = GTLib::Min(nextWidth, nextHeight);
                }


                Texture2D* texture;
                if (this->m_buffers.count > chainCount)
                {
                    texture = this->m_buffers[chainCount];
                }
                else
                {
                    texture = Renderer::CreateTexture2D();
                    Renderer::SetTexture2DFilter(texture, TextureFilter_Nearest, TextureFilter_Nearest);

                    this->m_buffers.PushBack(texture);
                }

                // Texture data has changed, so the renderer needs to know.
                assert(texture != nullptr);
                {
                    texture->SetData(nextWidth, nextHeight, GTLib::ImageFormat_R16F);
                    Renderer::PushTexture2DData(texture);
                }


                // We need to create a shader for this link. The shader we create depends on where in the chain this link is.
                bool encodeLuminance = false;
                bool decodeLuminance = false;
                if (chainCount == 0)
                {
                    // It's the first in the chain.
                    encodeLuminance = true;
                }
                else
                {
                    if (nextWidth == 1 && nextHeight == 1)
                    {
                        // It's the last in the chain.
                        decodeLuminance = true;
                    }
                }

                this->m_shaders.PushBack(this->CreateShader(prevWidth, prevHeight, encodeLuminance, decodeLuminance));


                prevWidth  = nextWidth;
                prevHeight = nextHeight;
                ++chainCount;
            }

            // Left-over textures need to be deleted.
            while (this->m_buffers.count > chainCount)
            {
                auto textureToDelete = this->m_buffers.GetBack();
                assert(textureToDelete != nullptr);
                {
                    Renderer::DeleteTexture2D(textureToDelete);
                    this->m_buffers.PopBack();
                }
            }
        }

        return true;
    }

    Shader* DefaultSceneRenderer_LuminanceChain::CreateShader(unsigned int inputTextureWidth, unsigned int inputTextureHeight, bool encodeLuminance, bool decodeLuminance)
    {
        // Vertex Shader.
        GTLib::String vertexSource
        (
            "#version 120\n"

            "attribute vec2 VertexInput_Position;\n"
            "attribute vec2 VertexInput_TexCoord;\n"

            "varying vec2 VertexOutput_Position;\n"
            "varying vec2 VertexOutput_TexCoord;\n"
            
            "void main()\n"
            "{\n"
            "    VertexOutput_Position = VertexInput_Position;\n"
            "    VertexOutput_TexCoord = VertexInput_TexCoord;\n"
            
            "    gl_Position = vec4(VertexInput_Position, 0.0, 1.0);\n"
            "}"
        );


        // Fragment Shader.
        GTLib::String fragmentSource
        (
            "#version 120\n"

            "varying vec2 VertexOutput_Position;\n"
            "varying vec2 VertexOutput_TexCoord;\n"
            
            "uniform sampler2D InputTexture;\n"
        );

        if (decodeLuminance)
        {
            fragmentSource.Append
            (
                "uniform vec2 Exposure;\n"      // x = exposure; y = 1.0 / (exposure + 1) * exposure
            );
        }


        fragmentSource.Append
        (
            "void main()\n"
            "{\n"
            "    float luminance = 0.0;\n"
        );

        if (encodeLuminance)
        {
            fragmentSource.Append
            (
                "    vec4 totalColour = vec4(0.0, 0.0, 0.0, 0.0);\n"
            );
        }

        unsigned int xSize = GTLib::Min(4U, inputTextureWidth);
        unsigned int ySize = GTLib::Min(4U, inputTextureHeight);

        for (unsigned int x = 0; x < xSize; ++x)
        {
            for (unsigned int y = 0; y < ySize; ++y)
            {
                float xOffset = 1.0f / static_cast<float>(inputTextureWidth)  * static_cast<float>(x);
                float yOffset = 1.0f / static_cast<float>(inputTextureHeight) * static_cast<float>(y);

                if (encodeLuminance)
                {
                    fragmentSource.AppendFormatted
                    (
                        "    totalColour += texture2D(InputTexture, VertexOutput_TexCoord + vec2(%f, %.f));\n", xOffset, yOffset
                    );
                }
                else
                {
                    fragmentSource.AppendFormatted
                    (
                        "    luminance += texture2D(InputTexture, VertexOutput_TexCoord + vec2(%f, %f)).r;\n", xOffset, yOffset
                    );
                }
            }
        }

        if (encodeLuminance)
        {
            fragmentSource.Append
            (
                "    luminance = dot(vec4(0.2126, 0.7152, 0.0722, 0.0), totalColour);\n"
            );
        }


        fragmentSource.AppendFormatted
        (
            "    luminance /= %f;\n", static_cast<float>(xSize * ySize)
        );


        if (decodeLuminance)
        {
            fragmentSource.Append
            (
                "    luminance = min(5.0, Exposure.y * (Exposure.x / luminance + 1.0));\n"
                
                // The above line is an optimized version of this:
                //"    luminance = min(5.0, Exposure * (Exposure / luminance + 1.0) / (Exposure + 1.0));\n"
            );
        }


        fragmentSource.AppendFormatted
        (
            "    gl_FragData[0] = vec4(luminance, 0.0, 0.0, 0.0);\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    void DefaultSceneRenderer_LuminanceChain::DeleteAllShaders()
    {
        for (size_t iShader = 0; iShader < this->m_shaders.count; ++iShader)
        {
            Renderer::DeleteShader(this->m_shaders[iShader]);
        }

        this->m_shaders.Clear();
    }

    void DefaultSceneRenderer_LuminanceChain::DeleteAllBuffers()
    {
        for (size_t iBuffer = 0; iBuffer < this->m_buffers.count; ++iBuffer)
        {
            Renderer::DeleteTexture2D(this->m_buffers[iBuffer]);
        }

        this->m_buffers.Clear();
    }
}
