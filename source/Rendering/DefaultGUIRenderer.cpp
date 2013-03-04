// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/DefaultGUIRenderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#include <GTGUI/Server.hpp>


namespace GTEngine
{
    DefaultGUIRenderer::DefaultGUIRenderer()
        : shader(ShaderLibrary::GetGUIShader()), vertexArray(Renderer2::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P2T2C4)),
          viewportWidth(0), viewportHeight(0), projection(0)
    {
        assert(shader      != nullptr);
        assert(vertexArray != nullptr);
    }

    DefaultGUIRenderer::~DefaultGUIRenderer()
    {
        Renderer2::DeleteVertexArray(this->vertexArray);

        for (size_t i = 0; i < this->textures.count; ++i)
        {
            Renderer2::DeleteTexture2D(this->textures.buffer[i]->value);
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
        Renderer2::EnableScissorTest();
        Renderer2::DisableDepthTest();
        Renderer2::DisableDepthWrites();


        // The viewport must be set each time.
        Renderer2::SetViewport(0, 0, this->viewportWidth, this->viewportHeight);

        // We want to render to the main framebuffer and use the main GUI shader.
        Renderer2::SetCurrentFramebuffer(nullptr);
        Renderer2::SetCurrentShader(this->shader);


        // We need to set the projection uniform on the main shader. Slightly more efficient if we do this after making it current.
        this->shader->SetParameter("Projection", this->projection);
        {
            Renderer2::PushShaderPendingProperties(*this->shader);
        }
        this->shader->ClearPendingParameters();


        // TODO: Remove this!
        Renderer2::SetClearColour(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer2::SetClearDepth(1.0f);
        Renderer2::Clear(BufferType_Colour | BufferType_Depth);
    }

    void DefaultGUIRenderer::End()
    {
        // Some state needs to be restored as per protocol.
        Renderer2::DisableScissorTest();
        Renderer2::DisableBlending();
        Renderer2::EnableDepthTest();
        Renderer2::EnableDepthWrites();
    }

    void DefaultGUIRenderer::SetScissor(int x, int y, unsigned int width, unsigned int height)
    {
        // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
        Renderer2::SetScissor(x, this->viewportHeight - (y + height), width, height);
    }

    void DefaultGUIRenderer::Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount, float offsetX, float offsetY, const GTImage::Image* image, bool enableOpacity)
    {
        auto texture = this->AcquireTexture2DFromImage(image);

        this->shader->SetParameter("Offset",  offsetX, offsetY);
        this->shader->SetParameter("Texture", texture);
        {
            Renderer2::PushShaderPendingProperties(*this->shader);
        }
        this->shader->ClearPendingParameters();


        if (enableOpacity)
        {
            Renderer2::EnableBlending();
            Renderer2::SetBlendFunction(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);
        }
        else
        {
            Renderer2::DisableBlending();  // Ensure blending is disabled for performance.
        }

        // We're going to do something a little inefficient here until we can improve it later. We'll use a single vertex array and update the data for every
        // call to this function.
        this->vertexArray->SetData(vertices, vertexCount, indices, indexCount);
        Renderer2::PushVertexArrayData(*this->vertexArray);
        Renderer2::Draw(*vertexArray);
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
            auto newTexture = Renderer2::CreateTexture2D();
            
            if (image != nullptr)
            {
                newTexture->SetData(image->GetWidth(), image->GetHeight(), image->GetFormat(), image->GetBaseMipmapData());
            }
            else
            {
                uint8_t texel[] = {0xFF, 0xFF, 0xFF};
                newTexture->SetData(1, 1, GTImage::ImageFormat_RGB8, texel);
            }
            
            Renderer2::PushTexture2DData(*newTexture);
            Renderer2::SetTexture2DFilter(*newTexture, TextureFilter_Nearest, TextureFilter_Nearest);


            // We don't need the local data, so it should be deleted. If we don't delete, we'll have a copy in GTGUI, here, and in the renderer. Removing one is nice.
            newTexture->DeleteLocalData();
            

            this->textures.Add(image, newTexture);
            return newTexture;
        }
    }
}