// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/DefaultGUIRenderer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#include <GTGUI/Server.hpp>


namespace GTEngine
{
    DefaultGUIRenderer::DefaultGUIRenderer()
        : shader(ShaderLibrary::GetGUIShader()),
          viewportWidth(0), viewportHeight(0), projection(0),
          textures(),
          vertexArray(GTEngine::Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P2T2C4))
    {
        assert(shader      != nullptr);
        assert(vertexArray != nullptr);
    }

    DefaultGUIRenderer::~DefaultGUIRenderer()
    {
        GTEngine::Renderer::DeleteVertexArray(this->vertexArray);

        for (size_t i = 0; i < this->textures.count; ++i)
        {
            GTEngine::Renderer::DeleteTexture2D(this->textures.buffer[i]->value);
        }
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
        GTEngine::Renderer::SetCurrentShader(this->shader);


        // We need to set the projection uniform on the main shader. Slightly more efficient if we do this after making it current.
        this->shader->SetUniform("Projection", this->projection);
        GTEngine::Renderer::PushPendingUniforms(*this->shader);
    }

    void DefaultGUIRenderer::End()
    {
        // Some state needs to be restored as per protocol.
        GTEngine::Renderer::DisableScissorTest();
        GTEngine::Renderer::DisableBlending();
        GTEngine::Renderer::EnableDepthTest();
        GTEngine::Renderer::EnableDepthWrites();
    }

    void DefaultGUIRenderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
        GTEngine::Renderer::SetScissor(x, this->viewportHeight - (y + height), width, height);
    }

    void DefaultGUIRenderer::Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, float offsetX, float offsetY, const GTImage::Image* image, bool enableOpacity)
    {
        auto texture = this->AcquireTexture2DFromImage(image);

        // The shader needs to be make current in case an OnDraw event handler changes it. Setting uniforms is slightly more efficient if we bind it first.
        GTEngine::Renderer::SetCurrentShader(this->shader);
        this->shader->SetUniform("Offset",  offsetX, offsetY);
        this->shader->SetUniform("Texture", texture);
        GTEngine::Renderer::PushPendingUniforms(*this->shader);


        if (enableOpacity)
        {
            GTEngine::Renderer::EnableBlending();
            GTEngine::Renderer::SetBlendEquation(BlendEquation_Add);
            GTEngine::Renderer::SetBlendFunction(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
        }
        else
        {
            GTEngine::Renderer::DisableBlending();  // Ensure blending is disabled for performance.
        }

        // We're going to do something a little inefficient here until we can improve it later. We'll use a single vertex array and update the data for every
        // call to this function.
        this->vertexArray->SetData(vertices, vertexCount, indices, indexCount);
        GTEngine::Renderer::Draw(*vertexArray);
    }




    /////////////////////////////////////////
    // Private.

    Texture2D* DefaultGUIRenderer::AcquireTexture2DFromImage(const GTImage::Image* image)
    {
        auto iTexture = this->textures.Find(image);
        if (iTexture != nullptr)
        {
            return iTexture->value;
        }
        else
        {
            auto newTexture = GTEngine::Renderer::CreateTexture2D();
            
            if (image != nullptr)
            {
                newTexture->SetData(image->GetWidth(), image->GetHeight(), image->GetFormat(), image->GetBaseMipmapData());
            }
            else
            {
                uint8_t texel[] = {0xFF, 0xFF, 0xFF};
                newTexture->SetData(1, 1, GTImage::ImageFormat_RGB8, texel);
            }
            
            GTEngine::Renderer::PushTexture2DData(*newTexture);
            GTEngine::Renderer::SetTexture2DFilter(*newTexture, TextureFilter_Nearest, TextureFilter_Nearest);


            // We don't need the local data, so it should be deleted. If we don't delete, we'll have a copy in GTGUI, here, and in the renderer. Removing one is nice.
            newTexture->DeleteLocalData();
            

            this->textures.Add(image, newTexture);
            return newTexture;
        }
    }
}
