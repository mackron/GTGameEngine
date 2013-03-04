// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/GUIRenderer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Math.hpp>
#include <GTEngine/FontManager.hpp>
#include <GTGUI/Server.hpp>

namespace GTEngine
{
    /// A helper function for retrieving the texture object to use with the given GTImage::Image object.
    ///
    /// @remarks
    ///     If <image> is null, this will return the default white texture. If <image> is not null and the texture has
    ///     already been created for it, the existing Texture2D object will be returned. Otherwise, a new Texture2D
    ///     object will be created and returned.
    Texture2D* GUIRenderer_AcquireTexture2DFromImage(const GTImage::Image* image);


    /// A list mapping a GTImage::Image object from GTGUI to a Texture2D object in GTEngine.
    GTCore::Map<const GTImage::Image*, Texture2D*> GUITextures;

    /// A white texture. We use this for quads that don't use a texture.
    Texture2D* GUIWhiteTexture = nullptr;


    /// The main shader.
    Shader* MainShader = nullptr;


    /// The projection matrix to use when drawing the GUI.
    glm::mat4 GUIProjection;

    /// We need to keep track of the height of the servers viewport so that we can do conversions.
    unsigned int GUIViewportWidth = 0, GUIViewportHeight = 0;


    void GUIRenderer::Initialise()
    {
        // This single pixel is the data for our white texture. It's important that this is an RGB8 format and not RGBA8. Reason is because we use
        // the presence of an alpha channel in determining whether or not to enable transparency.
        const uint8_t whiteTexel[3] = {0xFF, 0xFF, 0xFF};
        GUIWhiteTexture = new Texture2D(1, 1, GTImage::ImageFormat_RGB8, &whiteTexel);

        // We need to create the main shader.
        MainShader = ShaderLibrary::GetGUIShader();
    }

    void GUIRenderer::Uninitialise()
    {
        delete GUIWhiteTexture;
        GUIWhiteTexture = nullptr;

        for (size_t i = 0; i < GUITextures.count; ++i)
        {
            delete GUITextures.buffer[i]->value;
        }
        GUITextures.Clear();
    }

    void GUIRenderer::DrawGUI(const GTGUI::Server &server)
    {
        assert(MainShader != nullptr);
        {
            // We'll set the projection matrix based on the server's working area. GTGUI uses a top-down projection, so we construct
            // our projection matrix to match.
            unsigned int viewportWidth, viewportHeight;
            server.GetViewportSize(viewportWidth, viewportHeight);
            if (viewportWidth != GUIViewportWidth || viewportHeight != GUIViewportHeight)
            {
                GUIViewportWidth  = viewportWidth;
                GUIViewportHeight = viewportHeight;
                GUIProjection     = glm::ortho(0.0f, static_cast<float>(GUIViewportWidth), static_cast<float>(GUIViewportHeight), 0.0f, 0.0f, -1.0f);
            }
        
            Renderer::SetViewport(0, 0, viewportWidth, viewportHeight);

            Renderer::EnableScissorTest();
            Renderer::DisableDepthTest();
            Renderer::DisableDepthWrites();

            // Here we set a few shader parameters that only need to be set once. We need to do this for all shaders. Cases like this is where a
            // uniform buffer would come in real handy. I must look into that...
            MainShader->SetParameter("Projection", GUIProjection);


            //server.ExecuteFrontRCQueue();

            // Here we reset the applicable properties.
            Renderer::DisableScissorTest();
            Renderer::DisableBlending();
            Renderer::EnableDepthTest();
            Renderer::EnableDepthWrites();
        }
    }

    void GUIRenderer::EnableGUIState()
    {
        Renderer::SetViewport(0, 0, GUIViewportWidth, GUIViewportHeight);

        Renderer::EnableScissorTest();
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();
    }
}

#if 0
// These are GTGUI rendering command implementations.
void GTGUI::RCSetScissorRect::Execute()
{
    // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
    unsigned int width  = rect.right  - rect.left;
    unsigned int height = rect.bottom - rect.top;
    int x = rect.left;
    int y = GTEngine::GUIViewportHeight - rect.bottom;

    GTEngine::Renderer::SetScissor(x, y, width, height);
}

void GTGUI::RCDraw::Execute()
{
    auto texture = GTEngine::GUIRenderer_AcquireTexture2DFromImage(this->image);

    GTEngine::Renderer::SetShader(GTEngine::MainShader);
    GTEngine::Renderer::SetShaderParameter("Offset",  this->offsetX, this->offsetY);
    GTEngine::Renderer::SetShaderParameter("Texture", texture);

    if (this->enableOpacity)
    {
        GTEngine::Renderer::EnableAlphaBlending();
    }
    else
    {
        GTEngine::Renderer::DisableBlending();  // Ensure blending is disabled for performance.
    }

    GTEngine::Renderer::Draw(this->vertices, this->indices, this->indexCount, GTEngine::VertexFormat::P2T2C4);
}


namespace GTEngine
{
    Texture2D* GUIRenderer_AcquireTexture2DFromImage(const GTImage::Image* image)
    {
        if (image != nullptr)
        {
            auto iTexture = GUITextures.Find(image);
            if (iTexture != nullptr)
            {
                return iTexture->value;
            }
            else
            {
                auto newTexture = new Texture2D(image->GetWidth(), image->GetHeight(), image->GetFormat(), image->GetBaseMipmapData());
                newTexture->KeepClientSideData(true);
                newTexture->SetFilter(TextureFilter_Nearest, TextureFilter_Nearest);
                newTexture->GenerateMipmaps();


                GUITextures.Add(image, newTexture);

                return newTexture;
            }
        }
        else
        {
            return GUIWhiteTexture;
        }
    }
}
#endif
