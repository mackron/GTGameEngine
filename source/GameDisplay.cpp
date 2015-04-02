// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameDisplay.hpp>
#include <GTGameEngine/GameDisplayRenderingCallback.hpp>

namespace GT
{
    GameDisplay::GameDisplay(GraphicsInterface &graphicsInterface)
        : m_graphicsInterface(graphicsInterface),
          m_renderingCallback(nullptr)
    {
    }

    GameDisplay::~GameDisplay()
    {
    }


	GraphicsInterface & GameDisplay::GetGraphicsInterface()
    {
        return m_graphicsInterface;
    }


    void GameDisplay::SetRenderingCallback(GameDisplayRenderingCallback* renderingCallback)
    {
        m_renderingCallback = renderingCallback;
    }

    GameDisplayRenderingCallback* GameDisplay::GetRenderingCallback()
    {
        return m_renderingCallback;
    }
}