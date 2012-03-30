
#include <GTEngine/Rendering/GUIRenderer.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Rendering/Shader.hpp>
#include <GTEngine/Rendering/Texture2D.hpp>
#include <GTEngine/Rendering/VertexArray.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/Math.hpp>
#include <GTEngine/FontManager.hpp>

namespace GTEngine
{
    /// A white texture. We use this for quads that don't use a texture.
    Texture2D *GUIWhiteTexture = nullptr;

    /// The array containing a quad's vertex data. This changes dynamically.
    float GUIQuadVertices[] =
    {
        0.0f,  0.0f,
        0.0f,  0.0f,

        0.0f,  0.0f,
        1.0f,  0.0f,

        0.0f,  0.0f,
        1.0f,  1.0f,

        0.0f,  0.0f,
        0.0f,  1.0f
    };

    /// The array containing a quad's indices. This is constant for all quads.
    unsigned int GUIQuadIndices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    /// The projection matrix to use when drawing the GUI.
    glm::mat4 GUIProjection;

    /// We need to keep track of the height of the servers viewport so that we can do conversions.
    unsigned int GUIViewportWidth = 0, GUIViewportHeight = 0;


    void GUIRenderer::Initialise()
    {
        // Note how we don't create any shaders in the constructor. This is because we can't guarantee that they have been loaded by the shader
        // library at this point.

        // This single pixel is the data for our white texture.
        uint32_t whiteTexel = 0xFFFFFFFF;
        GUIWhiteTexture = new Texture2D(1, 1, GTImage::ImageFormat_RGBA8, &whiteTexel);
    }

    void GUIRenderer::Uninitialise()
    {
        delete GUIWhiteTexture;
    }

    void GUIRenderer::DrawGUI(const GTGUI::Server &server)
    {
        // We'll set the projection matrix based on the server's working area. GTGUI uses a top-down projection, so we construct
        // our projection matrix to match.
        unsigned int viewportWidth, viewportHeight;
        server.GetViewportSize(viewportWidth, viewportHeight);
        if (viewportWidth != GUIViewportWidth || viewportHeight != GUIViewportHeight)
        {
            GUIViewportWidth  = viewportWidth;
            GUIViewportHeight = viewportHeight;
            GUIProjection     = glm::ortho(0.0f, (float)GUIViewportWidth, (float)GUIViewportHeight, 0.0f, 0.0f, -1.0f);
        }
        
        Renderer::SetViewport(0, 0, viewportWidth, viewportHeight);

        Renderer::EnableScissorTest();
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();

        Renderer::SetShader(ShaderLibrary::GetGUITextShader());
        Renderer::SetShaderParameter("Projection", GUIProjection);

        Renderer::SetShader(ShaderLibrary::GetGUIQuadShader());
        Renderer::SetShaderParameter("Projection", GUIProjection);
        Renderer::SetShaderParameter("Texture",    GUIWhiteTexture);

        server.ExecuteFrontRCQueue();

        // Here we reset the applicable properties.
        Renderer::DisableScissorTest();
        Renderer::DisableBlending();
        Renderer::EnableDepthTest();
        Renderer::EnableDepthWrites();
    }

    void GUIRenderer::EnableGUIState()
    {
        Renderer::SetViewport(0, 0, GUIViewportWidth, GUIViewportHeight);

        Renderer::EnableScissorTest();
        Renderer::DisableDepthTest();
        Renderer::DisableDepthWrites();
    }
}

// These are GTGUI rendering command implementations.
void GTGUI::RCSetScissorRect::Execute()
{
    // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
    unsigned int width  = rect.right - rect.left;
    unsigned int height = rect.bottom - rect.top;
    int x = rect.left;
    int y = GTEngine::GUIViewportHeight - rect.bottom;

    GTEngine::Renderer::SetScissor(x, y, width, height);
}

void GTGUI::RCDrawQuad::Execute()
{
    // For ease of use...
    float left   = static_cast<float>(rect.left);
    float right  = static_cast<float>(rect.right);
    float top    = static_cast<float>(rect.top);
    float bottom = static_cast<float>(rect.bottom);

    GTEngine::GUIQuadVertices[0]  = left;  GTEngine::GUIQuadVertices[1]  = bottom;
    GTEngine::GUIQuadVertices[4]  = right; GTEngine::GUIQuadVertices[5]  = bottom;
    GTEngine::GUIQuadVertices[8]  = right; GTEngine::GUIQuadVertices[9]  = top;
    GTEngine::GUIQuadVertices[12] = left;  GTEngine::GUIQuadVertices[13] = top;

    GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetGUIQuadShader());
    GTEngine::Renderer::SetShaderParameter("Texture", GTEngine::GUIWhiteTexture);
    GTEngine::Renderer::SetShaderParameter("Color",   colour.r, colour.g, colour.b, this->opacity);

    // Need to check if blending should be enabled.
    if (this->opacity > 0.0f && this->opacity < 1.0f)
    {
        GTEngine::Renderer::EnableAlphaBlending();
    }
    else
    {
        GTEngine::Renderer::DisableBlending();  // Ensure blending is disabled for performance.
    }

    // Only draw the quad if we can actually see it...
    if (this->opacity > 0.0f)
    {
        GTEngine::Renderer::Draw(GTEngine::GUIQuadVertices, GTEngine::GUIQuadIndices, 6, GTEngine::VertexFormat::P2T2);
    }
}

void GTGUI::RCDrawText::Execute()
{
    assert(this->renderingInfo != nullptr);

    // GTGUI is top-down, but the renderer uses bottom-up. We need to convert appropriately.
    unsigned int width  = clipRect.right  - clipRect.left;
    unsigned int height = clipRect.bottom - clipRect.top;
    int x = clipRect.left;
    int y = GTEngine::GUIViewportHeight - clipRect.bottom;

    GTEngine::Renderer::SetScissor(x, y, width, height);

    GTEngine::Renderer::EnableAlphaBlending();
    GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetGUITextShader());
    GTEngine::Renderer::SetShaderParameter("Offset",  offsetX, offsetY);
    GTEngine::Renderer::SetShaderParameter("Color",   colour.r, colour.g, colour.b, this->opacity);

    for (auto i = this->renderingInfo->sections.root; i != nullptr; i = i->next)
    {
        if (i->value->font != nullptr)
        {
            GTEngine::Renderer::SetShaderParameter("Texture", GTEngine::FontManager::GetTexture(*i->value->font));
            GTEngine::Renderer::Draw(i->value->vertices, i->value->indices, i->value->indexCount, GTEngine::VertexFormat::P2T2);
        }
    }
}
