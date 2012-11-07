
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
          currentState(nullptr), loadedStates()
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
        auto newImage = Texture2DLibrary::Acquire(fileName);
        if (newImage != nullptr)
        {
            this->currentImage = newImage;


            // Now we need to setup some state for the new model. If the model has already had it's state recorded, we can just restore it from the map.
            auto iState = this->loadedStates.Find(fileName);
            if (iState != nullptr)
            {
                this->currentState = iState->value;
            }
            else
            {
                this->currentState = new State;
                this->loadedStates.Add(fileName, this->currentState);
            }


            return true;
        }

        return false;
    }


    float Editor_ImageEditor::GetZoom() const
    {
        return (this->currentState != nullptr) ? this->currentState->zoom : 1.0f;
    }

    void Editor_ImageEditor::SetZoom(float zoom)
    {
        if (this->currentState != nullptr)
        {
            this->currentState->zoom = zoom;
        }
    }


    Game & Editor_ImageEditor::GetGame()
    {
        return this->editor.GetGame();
    }

    const Game & Editor_ImageEditor::GetGame() const
    {
        return this->editor.GetGame();
    }



    //////////////////////////////////////////////////////
    // ViewportEventHandler

    void Editor_ImageEditor::ViewportEventHandler::OnDraw(GTGUI::Element &element)
    {
        auto image = this->editor.GetCurrentImage();
        if (image != nullptr)
        {
            GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetTextured2DQuadShader());
            GTEngine::Renderer::SetShaderParameter("Projection", glm::ortho(0.0f, static_cast<float>(element.server.GetViewportWidth()), static_cast<float>(element.server.GetViewportHeight()), 0.0f, 0.0f, -1.0f));
            GTEngine::Renderer::SetShaderParameter("Texture",    image);
            

            unsigned int imageWidth  = image->GetWidth();
            unsigned int imageHeight = image->GetHeight();

            GTGUI::Rect viewportRect;
            element.GetAbsoluteRect(viewportRect);

            float viewportWidth  = static_cast<float>(viewportRect.right  - viewportRect.left);
            float viewportHeight = static_cast<float>(viewportRect.bottom - viewportRect.top);

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
