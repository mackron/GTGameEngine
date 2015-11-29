// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/ImageEditor/ImageEditor.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>
#include <GTEngine/IO.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4355)   // 'this' used in initialise list.
#endif

namespace GT
{
    ImageEditor::ImageEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath),
          m_viewportElement(nullptr), m_viewportEventHandler(*this),
          m_image(Texture2DLibrary::Acquire(absolutePath, GT::GetBasePath(absolutePath, relativePath).c_str())), m_zoom(1.0f)
    {
        if (m_image != nullptr)
        {
            auto &gui    = this->GetGUI();
            auto &script = this->GetScript();

            m_viewportElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='image-editor' />");
            assert(m_viewportElement != nullptr);
            {
                // The element has been created, but we need to execute a script to have it turn into a proper image editor.
                script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", m_viewportElement->id).c_str());
                assert(script.IsTable(-1));
                {
                    script.Push("ImageEditor");
                    script.GetTableValue(-2);
                    assert(script.IsFunction(-1));
                    {
                        script.PushValue(-2);   // <-- 'self'.
                        script.Push(this);      // <-- '_internalPtr'
                        script.Call(2, 0);
                    }
                }

                m_viewportElement->AttachEventHandler(m_viewportEventHandler);
            }
        }
    }

    ImageEditor::~ImageEditor()
    {
        /// The vertex array in the viewports event handler needs to be deleted.
        Renderer::DeleteVertexArray(m_viewportEventHandler.vertexArray);

        // The image needs to be unacquired.
        Texture2DLibrary::Unacquire(m_image);


        // We created the viewport element in the constructor so it needs to be deleted here.
        this->GetGUI().DeleteElement(m_viewportElement);
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ImageEditor::Show()
    {
        m_viewportElement->Show();
    }

    void ImageEditor::Hide()
    {
        m_viewportElement->Hide();
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
            this->vertexArray = Renderer::CreateVertexArray(VertexArrayUsage_Dynamic, VertexFormat::P2T2);

            unsigned int indices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };
            this->vertexArray->SetIndexData(indices, 6);

            Renderer::PushVertexArrayIndexData(*this->vertexArray);
        }


        assert(this->vertexArray != nullptr);
        {
            auto image = this->ownerEditor.GetImage();
            if (image != nullptr)
            {
                auto shader = ShaderLibrary::GetTextured2DQuadShader();
                assert(shader != nullptr);
                {
                    Renderer::SetCurrentShader(shader);
                    shader->SetUniform("Projection", glm::ortho(0.0f, static_cast<float>(element.server.GetViewportWidth()), static_cast<float>(element.server.GetViewportHeight()), 0.0f, 0.0f, -1.0f));
                    shader->SetUniform("Texture",    image);
                    Renderer::PushPendingUniforms(*shader);

            

                    unsigned int imageWidth  = image->GetWidth();
                    unsigned int imageHeight = image->GetHeight();

                    Rect<int> viewportRect;
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
                    

                    // We want to enable blending here. There is no need to disable yet because the GUI renderer will be doing that when it's finished here.
                    Renderer::EnableBlending();
                    Renderer::SetBlendFunction(BlendFunc_SourceAlpha, BlendFunc_OneMinusSourceAlpha);

                    // Now we can draw.
                    Renderer::Draw(*this->vertexArray);
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
