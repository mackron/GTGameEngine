// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GameDisplay.hpp>
#include <GTGameEngine/GameDisplayRenderingCallback.hpp>

namespace GT
{
    GameDisplay::GameDisplay(GPURenderingDevice &renderingDevice)
        : m_renderingDevice(renderingDevice),
            m_renderingCallback(nullptr)
    {
    }

    GameDisplay::~GameDisplay()
    {
    }


    GPURenderingDevice & GameDisplay::GetRenderingDevice()
    {
        return m_renderingDevice;
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