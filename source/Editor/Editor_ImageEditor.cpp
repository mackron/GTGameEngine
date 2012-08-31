
#include <GTEngine/Editor.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    Editor_ImageEditor::Editor_ImageEditor(Editor &editor)
        : editor(editor), currentImage(nullptr), GUI(), viewportEventHandler(*this),
          zoom(1.0f)
    {
    }

    Editor_ImageEditor::~Editor_ImageEditor()
    {
    }

    void Editor_ImageEditor::Startup()
    {
        auto &gui = this->editor.GetGame().GetGUI();

        this->GUI.Viewport = gui.GetElementByID("Editor_ImageEditor_Viewport");
        this->GUI.Viewport->AttachEventHandler(this->viewportEventHandler);
    }

    bool Editor_ImageEditor::LoadImage(const char* fileName)
    {
        this->currentImage = Texture2DLibrary::Acquire(fileName);
        
        return this->currentImage != nullptr;
    }



    //////////////////////////////////////////////////////
    // ViewportEventHandler

    void Editor_ImageEditor::ViewportEventHandler::OnDraw(GTGUI::Element &element)
    {
        auto image = this->editor.GetCurrentImage();
        if (image != nullptr)
        {
            GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetGUIQuadShader());
            GTEngine::Renderer::SetShaderParameter("Texture", image);
            GTEngine::Renderer::SetShaderParameter("Color",   1.0f, 1.0f, 1.0f, 1.0f);

            unsigned int imageWidth  = image->GetWidth();
            unsigned int imageHeight = image->GetHeight();

            GTGUI::Rect viewportRect;
            element.GetAbsoluteRect(viewportRect);

            unsigned int viewportWidth  = viewportRect.right  - viewportRect.left;
            unsigned int viewportHeight = viewportRect.bottom - viewportRect.top;

            float zoom = this->editor.GetZoom();

            float quadLeft   = (viewportWidth  * 0.5f) - (imageWidth  * 0.5f * zoom) + viewportRect.left;
            float quadRight  = (viewportWidth  * 0.5f) + (imageWidth  * 0.5f * zoom) + viewportRect.left;
            float quadTop    = (viewportHeight * 0.5f) - (imageHeight * 0.5f * zoom) + viewportRect.top;
            float quadBottom = (viewportHeight * 0.5f) + (imageHeight * 0.5f * zoom) + viewportRect.top;


            float quadVertices[] =
            {
                quadLeft,  quadBottom,
                0.0f,      0.0f,

                quadRight, quadBottom,
                1.0f,      0.0f,

                quadRight, quadTop,
                1.0f,      1.0f,

                quadLeft,  quadTop,
                0.0f,      1.0f
            };

            unsigned int quadIndices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };

            GTEngine::Renderer::Draw(quadVertices, quadIndices, 6, GTEngine::VertexFormat::P2T2);
        }
    }

    void Editor_ImageEditor::ViewportEventHandler::OnMouseWheel(GTGUI::Element &, int delta, int, int)
    {
        this->editor.SetZoom(this->editor.GetZoom() + (this->editor.GetZoom() * (0.1f * delta)));
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
