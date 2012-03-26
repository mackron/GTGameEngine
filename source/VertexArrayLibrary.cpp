
#include <GTEngine/VertexArrayLibrary.hpp>

namespace GTEngine
{
    static VertexArray * FullscreenQuad = nullptr;

    VertexArray * VertexArrayLibrary::GetFullscreenQuadVA()
    {
        if (FullscreenQuad == nullptr)
        {
            FullscreenQuad = new VertexArray(VertexArrayUsage_Static, VertexFormat::P2);
            
            float vertices[] =
            {
                -1.0f, -1.0f,
                 1.0f, -1.0f,
                 1.0f,  1.0f,
                -1.0f,  1.0f,
            };

            unsigned int indices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };
            
            FullscreenQuad->SetData(vertices, 4, indices, 6);
        }

        return FullscreenQuad;
    }
}