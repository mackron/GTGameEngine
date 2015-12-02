// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor/Editor3DViewportEventHandler.hpp>
#include <GTGE/Game.hpp>
#include <GTGE/SceneViewport.hpp>
#include <GTGE/ShaderLibrary.hpp>

namespace GT
{
    Editor3DViewportEventHandler::Editor3DViewportEventHandler(Game &game, SceneViewport &viewport)
        : game(game), viewport(viewport),
          mousePosX(0), mousePosY(0),
          isMouseControlsEnabled(true)
    {
    }

    Editor3DViewportEventHandler::~Editor3DViewportEventHandler()
    {
    }

    
    void Editor3DViewportEventHandler::EnableMouseControls()
    {
        this->isMouseControlsEnabled = true;
    }

    void Editor3DViewportEventHandler::DisableMouseControls()
    {
        this->isMouseControlsEnabled = false;
    }


    void Editor3DViewportEventHandler::OnSize(GUIElement &element)
    {
        int newWidth  = Max(element.GetInnerWidth(),  1);
        int newHeight = Max(element.GetInnerHeight(), 1);

        this->viewport.Resize(newWidth, newHeight);

        auto camera = this->viewport.GetCameraNode()->GetComponent<CameraComponent>();
        camera->Set3DProjection(camera->perspective.fov, static_cast<float>(newWidth) / static_cast<float>(newHeight), camera->zNear, camera->zFar);
    }

    void Editor3DViewportEventHandler::OnDraw(GUIElement &element)
    {
        auto colourBuffer = this->viewport.GetColourBuffer();

        auto shader = ShaderLibrary::GetTextured2DQuadShader();
        assert(shader != nullptr);
        {
            Renderer::SetCurrentShader(shader);
            shader->SetUniform("Projection", glm::ortho(0.0f, static_cast<float>(element.server.GetViewportWidth()), static_cast<float>(element.server.GetViewportHeight()), 0.0f, 0.0f, -1.0f));
            shader->SetUniform("Texture",    colourBuffer);
            Renderer::PushPendingUniforms(*shader);

            
            Rect<int> viewportRect;
            element.GetAbsoluteRect(viewportRect);

            float quadLeft   = static_cast<float>(viewportRect.left);
            float quadRight  = static_cast<float>(viewportRect.right);
            float quadTop    = static_cast<float>(viewportRect.top);
            float quadBottom = static_cast<float>(viewportRect.bottom);



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

            Renderer::Draw(quadVertices, 4, quadIndices, 6, VertexFormat::P2T2);
        }
    }

    void Editor3DViewportEventHandler::OnMouseMove(GUIElement &, int x, int y)
    {
        if (!this->game.IsMouseCaptured())
        {
            this->mousePosX = x;
            this->mousePosY = y;
        }
    }

    void Editor3DViewportEventHandler::OnMouseWheel(GUIElement &, int delta, int x, int y)
    {
        (void)delta;
        (void)x;
        (void)y;
    }
}
