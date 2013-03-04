// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/ImageEditor/ImageEditor.hpp>
#include <GTEngine/Rendering/Renderer2.hpp>
#include <GTEngine/ShaderLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GTEngine
{
    ImageEditor::ImageEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          viewportElement(nullptr), viewportEventHandler(*this),
          image(Texture2DLibrary::Acquire(absolutePath, relativePath)), zoom(1.0f)
    {
        if (this->image != nullptr)
        {
            auto &gui = this->GetGUI();

            this->viewportElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' style='width:100%; height:100%; border:1px #222;' />");
            assert(this->viewportElement != nullptr);
            {
                this->viewportElement->AttachEventHandler(this->viewportEventHandler);
            }
        }
    }

    ImageEditor::~ImageEditor()
    {
        /// The vertex array in the viewports event handler needs to be deleted.
        Renderer2::DeleteVertexArray(this->viewportEventHandler.vertexArray);

        // The image needs to be unacquired.
        Texture2DLibrary::Unacquire(this->image);


        // We created the viewport element in the constructor so it needs to be deleted here.
        this->GetGUI().DeleteElement(this->viewportElement);
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ImageEditor::Show()
    {
        this->viewportElement->Show();
    }

    void ImageEditor::Hide()
    {
        this->viewportElement->Hide();
    }

    bool ImageEditor::Save()
    {
        return true;
    }




    //////////////////////////////////////////////////////
    // ViewportEventHandler

    void ImageEditor::ViewportEventHandler::OnDraw(GTGUI::Element &element)
    {
        if (this->vertexArray == nullptr)
        {
            this->vertexArray = Renderer2::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P2T2);

            unsigned int indices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };
            this->vertexArray->SetIndexData(indices, 6);

            Renderer2::PushVertexArrayIndexData(*this->vertexArray);
        }


        assert(this->vertexArray != nullptr);
        {
            auto image = this->ownerEditor.GetImage();
            if (image != nullptr)
            {
                auto shader = GTEngine::ShaderLibrary::GetTextured2DQuadShader();
                assert(shader != nullptr);
                {
                    Renderer2::SetCurrentShader(shader);

                    shader->SetParameter("Projection", glm::ortho(0.0f, static_cast<float>(element.server.GetViewportWidth()), static_cast<float>(element.server.GetViewportHeight()), 0.0f, 0.0f, -1.0f));
                    shader->SetParameter("Texture",    image);
                    {
                        Renderer2::PushShaderPendingProperties(*shader);
                    }
                    shader->ClearPendingParameters();

            

                    unsigned int imageWidth  = image->GetWidth();
                    unsigned int imageHeight = image->GetHeight();

                    GTGUI::Rect viewportRect;
                    element.GetAbsoluteRect(viewportRect);

                    float viewportWidth  = static_cast<float>(viewportRect.right  - viewportRect.left);
                    float viewportHeight = static_cast<float>(viewportRect.bottom - viewportRect.top);

                    float zoom = this->ownerEditor.GetZoom();

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
                    this->vertexArray->SetVertexData(quadVertices, 4);

                    Renderer2::PushVertexArrayVertexData(*this->vertexArray);
                    Renderer2::Draw(*this->vertexArray);
                }
            }
        }
    }

    void ImageEditor::ViewportEventHandler::OnMouseWheel(GTGUI::Element &, int delta, int, int)
    {
        this->ownerEditor.SetZoom(this->ownerEditor.GetZoom() + (this->ownerEditor.GetZoom() * (0.1f * delta)));
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
