// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/DefaultGUIRenderer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#include <GTGUI/Server.hpp>


namespace GTEngine
{
    DefaultGUIRenderer::DefaultGUIRenderer()
        : shader(ShaderLibrary::GetGUIShader()), shaderA8(ShaderLibrary::GetGUIShaderA8()),
          defaultTexture(nullptr),
          viewportWidth(0), viewportHeight(0), projection(0),
          currentOffsetX(0.0f), currentOffsetY(0.0f), currentShader(nullptr), currentTexture(nullptr), isBlendingEnabled(false),
          uniformsRequirePush(true)
    {
        assert(shader   != nullptr);
        assert(shaderA8 != nullptr);
        
        
        // We need a default texture.
        this->defaultTexture = GTEngine::Renderer::CreateTexture2D();
        
        uint8_t texel[] = {0xFF, 0xFF, 0xFF};
        this->defaultTexture->SetData(1, 1, GTImage::ImageFormat_RGB8, texel);

        GTEngine::Renderer::PushTexture2DData(this->defaultTexture);
        GTEngine::Renderer::SetTexture2DFilter(this->defaultTexture, TextureFilter_Nearest, TextureFilter_Nearest);
    }

    DefaultGUIRenderer::~DefaultGUIRenderer()
    {
    }


    void DefaultGUIRenderer::Begin(const GTGUI::Server &server)
    {
        // We need to do a little bit of initialisation.
        unsigned int newViewportWidth;
        unsigned int newViewportHeight;
        server.GetViewportSize(newViewportWidth, newViewportHeight);

        if (newViewportWidth != this->viewportWidth || newViewportHeight != this->viewportHeight)
        {
            this->viewportWidth  = newViewportWidth;
            this->viewportHeight = newViewportHeight;
            this->projection     = glm::ortho(0.0f, static_cast<float>(this->viewportWidth), static_cast<float>(this->viewportHeight), 0.0f, 0.0f, -1.0f);
        }


        // Some state needs to be one-time set.
        GTEngine::Renderer::EnableScissorTest();
        GTEngine::Renderer::DisableDepthTest();
        GTEngine::Renderer::DisableDepthWrites();

        // The viewport must be set each time.
        GTEngine::Renderer::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);

        // We want to render to the main framebuffer and use the main GUI shader.
        GTEngine::Renderer::SetCurrentFramebuffer(nullptr);

        // We need to set the projection uniform on the main shader. Slightly more efficient if we do this after making it current.
        this->shader->SetUniform("Projection", this->projection);
        this->shaderA8->SetUniform("Projection", this->projection);


        // Set the default state.
        this->RestoreCurrentState();
    }

    void DefaultGUIRenderer::End()
    {
        // Some state needs to be restored as per protocol.
        GTEngine::Renderer::DisableScissorTest();
        GTEngine::Renderer::EnableDepthTest();
        GTEngine::Renderer::EnableDepthWrites();

        if (this->isBlendingEnabled)
        {
            GTEngine::Renderer::DisableBlending();
        }
    }

    void DefaultGUIRenderer::BeginElementOnDrawEvent(GTGUI::Element &)
    {
    }

    void DefaultGUIRenderer::EndElementOnDrawEvent(GTGUI::Element &)
    {
        // Anything that may be been changed needs to be resetored.
        this->RestoreCurrentState();
    }

    void DefaultGUIRenderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
        GTEngine::Renderer::SetScissor(x, this->viewportHeight - (y + height), width, height);
    }

    void DefaultGUIRenderer::SetOffset(float offsetX, float offsetY)
    {
        this->currentOffsetX = offsetX;
        this->currentOffsetY = offsetY;

        this->shader->SetUniform("Offset", offsetX, offsetY);
        this->shaderA8->SetUniform("Offset", offsetX, offsetY);
        this->uniformsRequirePush = true;
    }

    void DefaultGUIRenderer::SetTexture(GTGUI::ImageHandle image)
    {
        if (image == 0)
        {
            this->currentTexture = this->defaultTexture;
        }
        else
        {
            this->currentTexture = reinterpret_cast<Texture2D*>(image);
        }
        
        
        if (this->currentTexture->GetFormat() == GTImage::ImageFormat_R8)
        {
            this->SetCurrentShader(this->shaderA8);
            this->shaderA8->SetUniform("Texture", this->currentTexture);
        }
        else
        {
            this->SetCurrentShader(this->shader);
            this->shader->SetUniform("Texture", this->currentTexture);
        }
        
        
        this->uniformsRequirePush = true;
    }

    void DefaultGUIRenderer::EnableBlending()
    {
        this->isBlendingEnabled = true;

        GTEngine::Renderer::EnableBlending();
        GTEngine::Renderer::SetBlendEquation(BlendEquation_Add);
        GTEngine::Renderer::SetBlendFunction(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
    }

    void DefaultGUIRenderer::DisableBlending()
    {
        this->isBlendingEnabled = false;

        GTEngine::Renderer::DisableBlending();
    }

    void DefaultGUIRenderer::Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount)
    {
        // The shader needs to be make current in case an OnDraw event handler changes it. Setting uniforms is slightly more efficient if we bind it first.
        if (this->uniformsRequirePush)
        {
            GTEngine::Renderer::PushPendingUniforms(*this->shader);
            GTEngine::Renderer::PushPendingUniforms(*this->shaderA8);
            this->uniformsRequirePush = false;
        }

        // Draw.
        GTEngine::Renderer::Draw(vertices, vertexCount, indices, indexCount, VertexFormat::P2T2C4);
    }




    /////////////////////////////////////////
    // Private.

    void DefaultGUIRenderer::RestoreCurrentState()
    {
        GTEngine::Renderer::SetCurrentShader(this->currentShader);          // <-- Don't use this->SetCurrentShader() here.
        this->SetOffset(this->currentOffsetX, this->currentOffsetY);
        this->SetTexture(reinterpret_cast<GTGUI::ImageHandle>(this->currentTexture));

        if (this->isBlendingEnabled)
        {
            this->EnableBlending();
        }
        else
        {
            this->DisableBlending();
        }
    }
    
    void DefaultGUIRenderer::SetCurrentShader(Shader* shader)
    {
        if (this->currentShader != shader)
        {
            GTEngine::Renderer::SetCurrentShader(shader);
            this->currentShader = shader;
        }
    }
}
