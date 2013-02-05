// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/Editor3DViewportEventHandler.hpp>
#include <GTEngine/Game.hpp>
#include <GTEngine/SceneViewport.hpp>
#include <GTEngine/ShaderLibrary.hpp>

namespace GTEngine
{
    Editor3DViewportEventHandler::Editor3DViewportEventHandler(Game &game, SceneViewport &viewport)
        : game(game), viewport(viewport),
          mousePosX(0), mousePosY(0)
    {
    }

    Editor3DViewportEventHandler::~Editor3DViewportEventHandler()
    {
    }



    void Editor3DViewportEventHandler::OnSize(GTGUI::Element &element)
    {
        int newWidth  = GTCore::Max(element.GetInnerWidth(),  1);
        int newHeight = GTCore::Max(element.GetInnerHeight(), 1);

        this->viewport.Resize(newWidth, newHeight);

        auto camera = this->viewport.GetCameraNode()->GetComponent<GTEngine::CameraComponent>();
        camera->Set3DProjection(90.0f, static_cast<float>(newWidth) / static_cast<float>(newHeight), 0.1f, 1000.0f);
    }

    void Editor3DViewportEventHandler::OnDraw(GTGUI::Element &element)
    {
        auto colourBuffer = this->viewport.GetColourBuffer();

        GTEngine::Renderer::SetShader(GTEngine::ShaderLibrary::GetTextured2DQuadShader());
        GTEngine::Renderer::SetShaderParameter("Projection", glm::ortho(0.0f, static_cast<float>(element.server.GetViewportWidth()), static_cast<float>(element.server.GetViewportHeight()), 0.0f, 0.0f, -1.0f));
        GTEngine::Renderer::SetShaderParameter("Texture",    colourBuffer);

        GTGUI::Rect viewportRect;
        element.GetAbsoluteRect(viewportRect);

        float quadLeft   = static_cast<float>(viewportRect.left);
        float quadRight  = static_cast<float>(viewportRect.right);
        float quadTop    = static_cast<float>(viewportRect.top);
        float quadBottom = static_cast<float>(viewportRect.bottom);

        // All we do is draw a quad over the viewport area.
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

    void Editor3DViewportEventHandler::OnLMBDown(GTGUI::Element &, int, int)
    {
        if (!this->game.IsMouseCaptured())
        {
            this->game.CaptureMouse();
        }
    }

    void Editor3DViewportEventHandler::OnLMBUp(GTGUI::Element &, int, int)
    {
        if (!this->game.IsMouseButtonDown(GTCore::MouseButton_Right))
        {
            this->game.ReleaseMouse();
        }
    }

    void Editor3DViewportEventHandler::OnRMBDown(GTGUI::Element &, int, int)
    {
        if (!this->game.IsMouseCaptured())
        {
            this->game.CaptureMouse();
        }
    }

    void Editor3DViewportEventHandler::OnRMBUp(GTGUI::Element &, int, int)
    {
        if (!this->game.IsMouseButtonDown(GTCore::MouseButton_Left))
        {
            this->game.ReleaseMouse();
        }
    }

    void Editor3DViewportEventHandler::OnMouseMove(GTGUI::Element &, int x, int y)
    {
        this->mousePosX = x;
        this->mousePosY = y;
    }

    void Editor3DViewportEventHandler::OnMouseWheel(GTGUI::Element &, int delta, int x, int y)
    {
        (void)x;
        (void)y;

        this->viewport.GetCameraNode()->MoveForward(delta * 1.0f);
    }
}
